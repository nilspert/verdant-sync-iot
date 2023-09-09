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
const int MOISTURE_SENSOR_PIN = A0;             // Analog pin for moisture sensor readings
const int DIGITAL_MOISTURE_SENSOR_PIN = 10;     // Digital pin for moisture sensor power
const int DIGITAL_WATER_PUMP_PIN = 16;           // Digital pin for water pump power
const int DIGITAL_BMP280_PIN = 2;
const int I2C_D1 = 5;
const int I2C_D2 = 4;
const int SOIL_WET = 500;
const int SOIL_DRY = 750;

// Events
const char* INFO = "INFO";
const char* WARNING = "WARNING";
const char* ERROR = "ERROR";
const char* DEVICE = "DEVICE";

const char* ADD_BOARD_INFO_SUCCESS_MESSAGE = "Board information added successfully.";
const char* ADD_BOARD_INFO_ERROR_MESSAGE = "Failed to add board information.";

const char* ADD_AUTHORIZED_DEVICE_SUCCESS_MESSAGE = "Device successfully added to authorized devices.";
const char* ADD_AUTHORIZED_DEVICE_PENDING_MESSAGE = "Device is pending authorization. Data sending disabled.";
const char* ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE = "Failed to add device to authorized devices";

const char* SEND_TEMPERATURE_SUCCESS_MESSAGE = "Temperature data sent successfully.";
const char* SEND_TEMPERATURE_ERROR_MESSAGE = "Failed to send temperature data.";

const char* SEND_SOIL_MOISTURE_SUCCESS_MESSAGE = "Soil moisture data sent successfully.";
const char* SEND_SOIL_MOISTURE_ERROR_MESSAGE = "Failed to send soil moisture data.";

ApiManager apiManager;
EventModule eventModule;
Adafruit_BMP280 bmp;

unsigned long previousMillis = 0;
unsigned long waterPumpActivatedMillis = 0;
bool waterPumpActivated = false;

void DeviceManager::setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    Wire.begin(I2C_D2, I2C_D1);
    bmp.begin(0x76);
    pinMode(DIGITAL_MOISTURE_SENSOR_PIN, OUTPUT); // Set the digital pin as an output
    pinMode(DIGITAL_WATER_PUMP_PIN, OUTPUT); // Set the digital pin as an output
    pinMode(DIGITAL_BMP280_PIN, OUTPUT); // Set the digital pin as an output. 
    digitalWrite(DIGITAL_MOISTURE_SENSOR_PIN, LOW); // Initially keep the sensor OFF
    digitalWrite(DIGITAL_WATER_PUMP_PIN, LOW);
    digitalWrite(DIGITAL_BMP280_PIN, HIGH); // Selects BMP280 I2C interface.
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

void DeviceManager::handleEvent(const char* severity, const char* message) {
    eventModule.createAndEnqueueEvent(getCurrentTimeAsString(), BOARD_NAME, severity, DEVICE, message, getNetworkName());
}

void DeviceManager::registerDeviceForAuthorization(String boardId) {
    String networkName = getNetworkName();
    String localIp = getLocalIpAsString();
    if (apiManager.encryptAndSendDeviceRegistration(boardId, networkName)) {
        Serial.println("Device successfully registered.");
        handleEvent(INFO, ADD_AUTHORIZED_DEVICE_SUCCESS_MESSAGE);
        if(apiManager.encryptAndSendBoardInfo(boardId, networkName, localIp)) {
            Serial.println("Board info data sent successfully.");
            handleEvent(INFO, ADD_BOARD_INFO_SUCCESS_MESSAGE);
        } else {
            Serial.println("Failed to send board info data.");
            handleEvent(ERROR, ADD_BOARD_INFO_ERROR_MESSAGE);
        }
    } else {
        Serial.println("Failed to register device.");
        handleEvent(ERROR, ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE);
    }
}

int DeviceManager::readSoilMoistureSensor() {
    digitalWrite(DIGITAL_MOISTURE_SENSOR_PIN, HIGH); // Turn the sensor ON
    delay(10); // Allow power to settle
    int val = analogRead(MOISTURE_SENSOR_PIN); // Read the analog value from sensor
    digitalWrite(DIGITAL_MOISTURE_SENSOR_PIN, LOW); // Turn the sensor OFF
    return val; // Return analog moisture value
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
        float temperature = bmp.readTemperature();
        Serial.print("Temperature: ");
        Serial.print(temperature);
        Serial.println(" *C");
        if (apiManager.encryptAndSendTemperature(temperature, boardId)) {
            Serial.println("Temperature data sent successfully.");
            handleEvent(INFO, SEND_TEMPERATURE_SUCCESS_MESSAGE);
        } else {
            Serial.println("Failed to send temperature data.");
            handleEvent(ERROR, SEND_TEMPERATURE_ERROR_MESSAGE);
        }
        
        // Read soil moisture
        int soilMoisture = readSoilMoistureSensor();
        if (apiManager.encryptAndSendSoilMoisture(soilMoisture, boardId)) {
            Serial.println("Soil moisture data sent successfully.");
            handleEvent(INFO, SEND_SOIL_MOISTURE_SUCCESS_MESSAGE);
        } else {
            Serial.println("Failed to send soil moisture data.");
            handleEvent(ERROR, SEND_SOIL_MOISTURE_ERROR_MESSAGE);
        }

        // Determine soil status
        if (soilMoisture < SOIL_WET) {
            Serial.println("Status: Soil is too wet");
        } else if (soilMoisture >= SOIL_WET && soilMoisture < SOIL_DRY) {
            Serial.println("Status: Soil moisture is perfect");
        } else {
            Serial.println("Status: Soil is too dry - time to water!");
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
