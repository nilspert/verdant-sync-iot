/**
 * File: device_manager.cpp
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains implementation of DeviceManager.
 * Provides functionality for main program.
 * - Modules initialization
 * - Device registration
 * - Sensor readings
 * - Water pump control
 */

#include "device_manager.h"
#include "../../config/config.h"
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../aes_module/aes_module.h"
#include "../firebase_module/firebase_module.h"
#include "../api_manager/api_manager.h"
#include "../event_module/event_module.h"

// Instances for managing API calls and events
ApiManager apiManager;
EventModule eventModule;

// Sensor and time-related variables
Adafruit_BMP280 bmp; // BMP280 sensor
DHT dht(DIGITAL_DHT22_PIN, DHT_TYPE); // DHT22 sensor

// Operation time tracking variables
unsigned long previousMillis = 0;
unsigned long waterPumpActivatedMillis = 0;

// Flags and initial sensor values
bool waterPumpActivated = false;
bool sensorReadingsDone = false;
int currentSoilMoisture = 1024;
float currentWaterTankLevel = -1.0;

// Setup function
void DeviceManager::setup() {
    Serial.begin(SERIAL_BAUD_RATE); // Initialize serial communication at the specified baud rate
    Wire.begin(I2C_D2, I2C_D1); // Initialize I2C communication with specified pins for BMP280
    bmp.begin(0x76); // Initialize BMP280 sensor with specified I2C address (0x76)
    dht.begin(); // Initialize DHT sensor

    // Set pin modes and set water pump to LOW as in OFF
    pinMode(DIGITAL_CD74HC4051E_CONTROL_PIN_1, OUTPUT);
    pinMode(DIGITAL_CD74HC4051E_CONTROL_PIN_2, OUTPUT);
    pinMode(DIGITAL_CD74HC4051E_CONTROL_PIN_3, OUTPUT);
    pinMode(DIGITAL_WATER_PUMP_PIN, OUTPUT);
    pinMode(DIGITAL_HC_SR04_TRIGGER_PIN, OUTPUT);  
    pinMode(DIGITAL_HC_SR04_ECHO_PIN, INPUT); 
    digitalWrite(DIGITAL_WATER_PUMP_PIN, LOW);

    initModules(); // Initialize modules

    // Check and register device if not added to Firebase
    String boardId = getBoardId();
    if (!isDeviceAuthorized(boardId)) {
        registerDeviceForAuthorization(boardId);
    }
}

// Function for initializing modules
void DeviceManager::initModules() {
    wifiModuleInit();
    timeModuleInit();
    aesModuleInit();
    firebaseModuleInit();
}

// Function for event creation
void DeviceManager::handleEvent(const char* severity, const char* message, const char* eventType) {
    // Call eventModule createAndEnqueueEvent to create and enqueue event for sending to firebase
    eventModule.createAndEnqueueEvent(getCurrentTimeAsString(), BOARD_NAME, severity, DEVICE, message, getNetworkName(), eventType);
}

// Function for registering device
void DeviceManager::registerDeviceForAuthorization(String boardId) {
    String networkName = getNetworkName();
    String localIp = getLocalIpAsString();

    // Register device and send registration data
    if (apiManager.encryptAndSendDeviceRegistration(boardId, networkName)) {
        Serial.println("Device registered.");

        // Send board info data
        if (apiManager.encryptAndSendBoardInfo(boardId, networkName, localIp)) {
            Serial.println("Board info sent.");
        } else {
            Serial.println("Failed to send board info.");
            handleEvent(ERROR, ADD_BOARD_INFO_ERROR_MESSAGE, BOARD_INFO);
        }
    } else {
        Serial.println("Failed to register device.");
        handleEvent(ERROR, ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE, REGISTRATION);
    }
}

// Function for reading and sending temperature data to firebase
void DeviceManager::readAndSendTemperature(String boardId, String networkName) {
    float temperature = dht.readTemperature(); // Read temperature from DHT22

    // Print temperature reading
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");

    // Send temperature data to Firebase
    if (apiManager.encryptAndSendTemperature(temperature, boardId, networkName)) {
        Serial.println("Temperature data sent successfully.");
    } else {
        Serial.println("Failed to send temperature data.");
        handleEvent(ERROR, SEND_TEMPERATURE_ERROR_MESSAGE, TEMPERATURE);
    }
}

// Function for reading and sending humidity data to firebase
void DeviceManager::readAndSendHumidity(String boardId, String networkName) {
    float humidity = dht.readHumidity(); // Read humidity from DHT22

    // Print humidity reading
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Send humidity data to Firebase
    if (apiManager.encryptAndSendHumidity(humidity, boardId, networkName)) {
        Serial.println("Humidity data sent successfully.");
    } else {
        Serial.println("Failed to send humidity data.");
        handleEvent(ERROR, SEND_HUMIDITY_ERROR_MESSAGE, HUMIDITY);
    }
}

// Function for reading and sending air pressure data to firebase
void DeviceManager::readAndSendAirPressure(String boardId, String networkName) {
    // Read air pressure from BMP280 and convert to hPa
    float airPressure = bmp.readPressure() / 100.0F;

    // Print air pressure reading
    Serial.print("Air pressure: ");
    Serial.print(airPressure);
    Serial.println(" hPa");

    // Send air pressure data to Firebase
    if (apiManager.encryptAndSendAirPressure(airPressure, boardId, networkName)) {
        Serial.println("Air pressure data sent successfully.");
    } else {
        Serial.println("Failed to send air pressure data.");
        handleEvent(ERROR, SEND_AIR_PRESSURE_ERROR_MESSAGE, AIR_PRESSURE);
    }
}

// Function for reading and sending luminosity data to firebase
void DeviceManager::readAndSendLuminosity(String boardId, String networkName) {
    int luminosity = readPhotoresistor(); // Read luminosity from photoresistor

    // Print luminosity reading
    Serial.print("Luminosity: ");
    Serial.print(luminosity);
    Serial.println(" %");

    // Send luminosity data to Firebase
    if (apiManager.encryptAndSendLuminosity(luminosity, boardId, networkName)) {
        Serial.println("Luminosity data sent successfully.");
    } else {
        Serial.println("Failed to send luminosity data.");
        handleEvent(ERROR, SEND_LUMINOSITY_ERROR_MESSAGE, LUMINOSITY);
    }
}

// Function for reading and sending water tank level data to firebase
float DeviceManager::readAndSendWaterTankLevel(String boardId, String networkName) {
    // Measure water tank level with HC_SR04
    // 10 µs HIGH voltage starts echo pulse
    digitalWrite(DIGITAL_HC_SR04_TRIGGER_PIN, LOW);
    delayMicroseconds(2);
    digitalWrite(DIGITAL_HC_SR04_TRIGGER_PIN, HIGH);
    delayMicroseconds(10);
    digitalWrite(DIGITAL_HC_SR04_TRIGGER_PIN, LOW);

    // Measure the duration of the echo pulse
    unsigned long duration = pulseIn(DIGITAL_HC_SR04_ECHO_PIN, HIGH);

    // Calculate the distance in centimeters
    float distance = (duration * 0.0343) / 2;

    // Check for out-of-range or error conditions
    if (distance < 2 || distance > MAX_DISTANCE_CM) {
        // Out of range or invalid measurement
        Serial.println("Out of range or invalid measurement");
        return -1.0;
    } else {
        // Print the measured distance
        Serial.print("Distance: ");
        Serial.print(distance);
        Serial.println(" cm");
    }

    // Send water tank level data to Firebase
    if (apiManager.encryptAndSendWaterTankLevel(distance, boardId, networkName)) {
        Serial.println("Water tank level data sent successfully.");
    } else {
        Serial.println("Failed to send water tank level data.");
        handleEvent(ERROR, SEND_WATER_TANK_LEVEL_ERROR_MESSAGE, WATER_TANK_LEVEL);
    }
    
    // Return value of distance
    return distance;
}

// Function for reading and sending soil moisture data to firebase
int DeviceManager::readAndSendSoilMoisture(String boardId, String networkName) {
    // Read soil moisture
    int soilMoisture = readSoilMoistureSensor();

    // Print soil moisture reading
    Serial.print("Soil moisture: ");
    Serial.print(soilMoisture);
    Serial.println(" %");

    // Send soil moisture data to firebase
    if (apiManager.encryptAndSendSoilMoisture(soilMoisture, boardId, networkName)) {
        Serial.println("Soil moisture data sent successfully.");
    } else {
        Serial.println("Failed to send soil moisture data.");
        handleEvent(ERROR, SEND_SOIL_MOISTURE_ERROR_MESSAGE, SOIL_MOISTURE);
    }

    // Return value of soil moisture
    return soilMoisture;
}

// Function for checking soil moisture status and decision for starting watering sequence
bool DeviceManager::checkSoilStatus(int soilMoisture) {
    bool startWateringSequence = false; // Return variable, defaults to false

    // If statement for checking soil status
    if (soilMoisture < SOIL_WET) {
        Serial.println(SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE);
        handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE, SOIL_MOISTURE_INFO);
    } else if (soilMoisture >= SOIL_WET && soilMoisture < SOIL_DRY) {
        Serial.println(SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE);
        handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE, SOIL_MOISTURE_INFO);
    } else {
        Serial.println(SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE);
        handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE, SOIL_MOISTURE_INFO);
        startWateringSequence = true; // If soil is dry, start watering sequence
    }

    // Return result
    return startWateringSequence;
}

// Function for reading soil moisture sensor value
int DeviceManager::readSoilMoistureSensor() {
    // Configure Multiplexer IC 74157 to select soil moisture sensor
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_1, LOW);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_2, LOW);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_3, HIGH);
    delay(10); // Short delay so that the sensor stabilizes
    int val = analogRead(ANALOG_OUTPUT_PIN); // Read the analog value from sensor
    return val; // Return analog soil moisture value
}

// Function for reading photoresistor value
int DeviceManager::readPhotoresistor() {
    // Configure Multiplexer IC 74157 to select photoresistor
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_1, LOW);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_2, HIGH);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_3, LOW);
    delay(10); // Short delay so that the sensor stabilizes
    int val = analogRead(ANALOG_OUTPUT_PIN); // Read the analog value from sensor
    return val; // Return analog luminosity value
}


// Function for activating water pump
void DeviceManager::activateWaterPump(bool activate) {
    digitalWrite(DIGITAL_WATER_PUMP_PIN, activate ? HIGH : LOW);
}

// Function for sending latest watering time to firebase
void DeviceManager::sendLatestWateringTime(String boardId, String networkName) {
    if (apiManager.encryptAndSendLatestWateringTime(getCurrentTimeAsString(), boardId, networkName)) {
        Serial.println("Watering time sent successfully.");
    } else {
        Serial.println("Failed to send watering time.");
        handleEvent(ERROR, SEND_LATEST_WATERING_TIME_ERROR_MESSAGE, LATEST_WATERING_TIME);
    }
}

// Function for sending water tank refill notification to firebase
void DeviceManager::sendWaterTankRefillNotification(String boardId, String networkName) {
    if (apiManager.encryptAndSendWaterTankRefillNotification(getCurrentTimeAsString(), boardId, networkName)) {
        Serial.println("Water tank refill notification sent successfully.");
    } else {
        Serial.println("Failed to send water tank refill notification.");
        handleEvent(ERROR, SEND_WATER_TANK_REFILL_NOTIFICATION_ERROR_MESSAGE, WATER_TANK_REFILL_NOTIFICATION);
    }
}

// Main loop function for DeviceManager
void DeviceManager::loop() {
    unsigned long currentMillis = millis(); // Current time in milliseconds since device started
    String boardId = getBoardId(); // Unique board identifier
    String networkName = getNetworkName(); // WiFi SSID

    // If device is authorized and LOOP_DELAY has passed, do sensor readings and data sending to firebase
    if ((currentMillis - previousMillis >= LOOP_DELAY) && isDeviceAuthorized(boardId)) {       
        // Read and send sensor data
        readAndSendTemperature(boardId, networkName);
        readAndSendHumidity(boardId, networkName);
        readAndSendAirPressure(boardId, networkName);
        readAndSendLuminosity(boardId, networkName);
        currentSoilMoisture = readAndSendSoilMoisture(boardId, networkName);
        currentWaterTankLevel = readAndSendWaterTankLevel(boardId, networkName);

        // Reset the timer
        sensorReadingsDone = true;
        previousMillis = currentMillis; 
    } else {
        // Process created events
        eventModule.loop();
    }

    // Check if sensor readings are done and soil status is dry
    if (sensorReadingsDone && checkSoilStatus(currentSoilMoisture)) {
        sensorReadingsDone = false;
        // Check if current water tank level is below minimum allowed level
        if (currentWaterTankLevel <= MINIMUM_WATER_TANK_LEVEL) {
            Serial.println("Activating water pump.");
            activateWaterPump(true);
            waterPumpActivatedMillis = currentMillis;
            waterPumpActivated = true;
        } else {
            // Send notification if water tank level is too low
            Serial.println("Water tank level is too low, please refill.");
            sendWaterTankRefillNotification(boardId, networkName);
        }
    }

    // Check if the water pump has been activated and stop it after the watering sequence
    if (waterPumpActivated && (currentMillis - waterPumpActivatedMillis >= WATERING_SEQUENCE)) {
        Serial.println("Stop!");
        waterPumpActivated = false;
        activateWaterPump(false);
        sendLatestWateringTime(boardId, networkName);
    }
}
