#include "device_manager.h"
#include "../../config/config.h"
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../aes_module/aes_module.h"
#include "../firebase_module/firebase_module.h"
#include "../api_manager/api_manager.h"
#include "../event_module/event_module.h"

// Constants and Configuration Settings
const int SERIAL_BAUD_RATE = 115200;
const int LOOP_DELAY = 10000;
const int WATERING_SEQUENCE = 2000;
const int ANALOG_OUTPUT_PIN = A0;               // Analog pin for CD74HC4051E
const int DIGITAL_WATER_PUMP_PIN = 16;           // Digital pin for water pump power
const int DIGITAL_DHT22_PIN = 2;
const int DIGITAL_CD74HC4051E_CONTROL_PIN_1 = 14;
const int DIGITAL_CD74HC4051E_CONTROL_PIN_2 = 12;
const int DIGITAL_CD74HC4051E_CONTROL_PIN_3 = 13;
const int I2C_D1 = 5;
const int I2C_D2 = 4;
const int SOIL_WET = 500;
const int SOIL_DRY = 750;

// Events
const char* INFO = "INFO";
const char* WARNING = "WARNING";
const char* ERROR = "ERROR";
const char* DEVICE = "DEVICE";

// Event types
const char* REGISTRATION = "0x0";
const char* BOARD_INFO = "0x1";
const char* TEMPERATURE = "0x2";
const char* HUMIDITY = "0x3";
const char* AIR_PRESSURE = "0x4";
const char* SOIL_MOISTURE = "0x5";
const char* SOIL_MOISTURE_INFO = "0x6";
const char* LUMINOSITY = "0x7";

const char* ADD_BOARD_INFO_ERROR_MESSAGE = "Failed to add board information.";
const char* ADD_AUTHORIZED_DEVICE_PENDING_MESSAGE = "Device is pending authorization. Data sending disabled.";
const char* ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE = "Failed to add device to authorized devices";
const char* SEND_TEMPERATURE_ERROR_MESSAGE = "Failed to send temperature data.";
const char* SEND_HUMIDITY_ERROR_MESSAGE = "Failed to send humidity data.";
const char* SEND_AIR_PRESSURE_ERROR_MESSAGE = "Failed to send air pressure data.";
const char* SEND_SOIL_MOISTURE_ERROR_MESSAGE = "Failed to send soil moisture data.";
const char* SEND_LUMINOSITY_ERROR_MESSAGE = "Failed to send luminosity data.";
const char* SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE = "Status: high soil moisture.";
const char* SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE = "Status: optimal soil moisture.";
const char* SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE = "Status: low soil moisture.";

ApiManager apiManager;
EventModule eventModule;
Adafruit_BMP280 bmp;
DHT dht(DIGITAL_DHT22_PIN, DHT_TYPE);

unsigned long previousMillis = 0;
unsigned long waterPumpActivatedMillis = 0;
bool waterPumpActivated = false;

void DeviceManager::setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Wire.begin(I2C_D2, I2C_D1);
    bmp.begin(0x76);
    dht.begin();
    pinMode(DIGITAL_CD74HC4051E_CONTROL_PIN_1, OUTPUT);
    pinMode(DIGITAL_CD74HC4051E_CONTROL_PIN_2, OUTPUT);
    pinMode(DIGITAL_CD74HC4051E_CONTROL_PIN_3, OUTPUT);
    pinMode(DIGITAL_WATER_PUMP_PIN, OUTPUT); // Set the digital pin as an output
    digitalWrite(DIGITAL_WATER_PUMP_PIN, LOW);
    initModules();
    String boardId = getBoardId();
    if (!isDeviceAddedToFirebase(boardId)) {
        registerDeviceForAuthorization(boardId);
    }
}

void DeviceManager::initModules() {
    wifiModuleInit();
    timeModuleInit();
    aesModuleInit();
    firebaseModuleInit();
}

void DeviceManager::handleEvent(const char* severity, const char* message, const char* eventType) {
    eventModule.createAndEnqueueEvent(getCurrentTimeAsString(), BOARD_NAME, severity, DEVICE, message, getNetworkName(), eventType);
}

void DeviceManager::registerDeviceForAuthorization(String boardId) {
    String networkName = getNetworkName();
    String localIp = getLocalIpAsString();
    if (apiManager.encryptAndSendDeviceRegistration(boardId, networkName)) {
        Serial.println("Device successfully registered.");
        if(apiManager.encryptAndSendBoardInfo(boardId, networkName, localIp)) {
            Serial.println("Board info data sent successfully.");
        } else {
            Serial.println("Failed to send board info data.");
            handleEvent(ERROR, ADD_BOARD_INFO_ERROR_MESSAGE, BOARD_INFO);
        }
    } else {
        Serial.println("Failed to register device.");
        handleEvent(ERROR, ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE, REGISTRATION);
    }
}

int DeviceManager::readSoilMoistureSensor() {
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_1, LOW);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_2, LOW);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_3, HIGH);
    delay(10); // Allow power to settle
    int val = analogRead(ANALOG_OUTPUT_PIN); // Read the analog value from sensor
    return val; // Return analog moisture value
}

int DeviceManager::readPhotoresistor() {
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_1, LOW);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_2, HIGH);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_3, LOW);
    delay(10); // Allow power to settle
    int val = analogRead(ANALOG_OUTPUT_PIN); // Read the analog value from sensor
    return val; // Return analog luminosity value
}

void DeviceManager::activateWaterPump(bool activate) {
    digitalWrite(DIGITAL_WATER_PUMP_PIN, activate ? HIGH : LOW);
}

void DeviceManager::loop() {
    unsigned long currentMillis = millis();
    String boardId = getBoardId();

    if ((currentMillis - previousMillis >= LOOP_DELAY) && isDeviceAuthorized(boardId)) {        
        Serial.println("Looping away...");

        // Read temperature
        float temperature = dht.readTemperature();
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" *C");
        if (apiManager.encryptAndSendTemperature(temperature, boardId)) {
            Serial.println("Temperature data sent successfully.");
        } else {
            Serial.println("Failed to send temperature data.");
            handleEvent(ERROR, SEND_TEMPERATURE_ERROR_MESSAGE, TEMPERATURE);
        }

        // Read humidity
        float humidity = dht.readHumidity();
        Serial.print("Humidity: ");
        Serial.print(humidity);
        Serial.println(" %");
        if (apiManager.encryptAndSendHumidity(humidity, boardId)) {
            Serial.println("Humidity data sent successfully.");
        } else {
            Serial.println("Failed to send humidity data.");
            handleEvent(ERROR, SEND_HUMIDITY_ERROR_MESSAGE, HUMIDITY);
        }

        // Read air pressure
        float airPressure = bmp.readPressure() / 100.0F; // Convert to hPa
        Serial.print("Air pressure: ");
        Serial.print(airPressure);
        Serial.println(" hPa");
        if (apiManager.encryptAndSendAirPressure(airPressure, boardId)) {
            Serial.println("Air pressure data sent successfully.");
        } else {
            Serial.println("Failed to send air pressure data.");
            handleEvent(ERROR, SEND_AIR_PRESSURE_ERROR_MESSAGE, AIR_PRESSURE);
        }
        
        // Read soil moisture
        int soilMoisture = readSoilMoistureSensor();
        Serial.print("Soil moisture: ");
        Serial.print(soilMoisture);
        Serial.println(" %");
        if (apiManager.encryptAndSendSoilMoisture(soilMoisture, boardId)) {
            Serial.println("Soil moisture data sent successfully.");
        } else {
            Serial.println("Failed to send soil moisture data.");
            handleEvent(ERROR, SEND_SOIL_MOISTURE_ERROR_MESSAGE, SOIL_MOISTURE);
        }

        int luminosity = readPhotoresistor();
        Serial.print("Luminosity: ");
        Serial.print(luminosity);
        Serial.println(" %");
        if (apiManager.encryptAndSendLuminosity(luminosity, boardId)) {
            Serial.println("Luminosity data sent successfully.");
        } else {
            Serial.println("Failed to send luminosity data.");
            handleEvent(ERROR, SEND_LUMINOSITY_ERROR_MESSAGE, LUMINOSITY);
        }

        // Determine soil status
        if (soilMoisture < SOIL_WET) {
            Serial.println(SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE);
            handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE, SOIL_MOISTURE_INFO);
        } else if (soilMoisture >= SOIL_WET && soilMoisture < SOIL_DRY) {
            Serial.println(SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE);
            handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE, SOIL_MOISTURE_INFO);
        } else {
            Serial.println(SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE);
            handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE, SOIL_MOISTURE_INFO);
            activateWaterPump(true);
            waterPumpActivatedMillis = currentMillis;
            waterPumpActivated = true;
        }

        previousMillis = currentMillis; // Reset the timer
    }

    if (waterPumpActivated && (currentMillis - waterPumpActivatedMillis >= WATERING_SEQUENCE)) {
        Serial.println("Stop!");
        waterPumpActivated = false;
        activateWaterPump(false);
    }

    eventModule.loop();
}
