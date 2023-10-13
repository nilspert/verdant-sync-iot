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
#include "../globals/globals.h"
#include "../sensor_manager/sensor_manager.h"

// Instances for managing API calls and events
EventModule eventModule;
SensorManager sensorManager;
ApiManager apiManager;

// Device and network configuration
String boardId = "";
String networkName = "";

// Operation time tracking variables
unsigned long previousSensorMillis = 0;
unsigned long previousSoilMoistureMillis = 0;
unsigned long waterPumpActivatedMillis = 0;

// Flags and initial sensor values
bool waterPumpActivated = false;
bool sensorReadingsDone = false;
bool startWateringSequence = false;
int currentSoilMoisture = 625;
float currentWaterTankLevel = -1.0;

// Setup function
void DeviceManager::setup() {
    Serial.begin(SERIAL_BAUD_RATE); // Initialize serial communication at the specified baud rate

    // Set pin modes and set water pump to LOW as in OFF
    pinMode(DIGITAL_CD74HC4051E_CONTROL_PIN_2, OUTPUT);
    pinMode(DIGITAL_CD74HC4051E_CONTROL_PIN_3, OUTPUT);
    pinMode(DIGITAL_SOIL_MOISTURE_SENSOR_PIN, OUTPUT);
    pinMode(DIGITAL_WATER_PUMP_PIN, OUTPUT);
    digitalWrite(DIGITAL_WATER_PUMP_PIN, LOW);
    digitalWrite(DIGITAL_SOIL_MOISTURE_SENSOR_PIN, LOW);

    initModules(); // Initialize modules

    // Set board related variables
    boardId = getBoardId(); // Unique board identifier
    networkName = getNetworkName(); // WiFi SSID

    // Check and register device if not added to Firebase
    if (!isDeviceAuthorized(boardId)) {
        registerDeviceForAuthorization(boardId);
    } else {
        Serial.println("Device is authorized.");
    }
}

// Function for initializing modules
void DeviceManager::initModules() {
    wifiModuleInit();
    timeModuleInit();
    aesModuleInit();
    firebaseModuleInit();
    sensorManager.setup();
}

// Function for registering device
void DeviceManager::registerDeviceForAuthorization(String boardId) {
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

// Function for checking soil moisture status and decision for starting watering sequence
bool DeviceManager::checkSoilStatus(int soilMoisture) {
    bool startWateringSequenceReturnValue = false; // Return variable, defaults to false

    // If statement for checking soil status
    if (soilMoisture < SOIL_WET_VALUE) {
        Serial.println(SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE);
        handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE, SOIL_MOISTURE_INFO);
    } else if (soilMoisture >= SOIL_WET_VALUE && soilMoisture < SOIL_DRY_VALUE) {
        Serial.println(SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE);
        handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE, SOIL_MOISTURE_INFO);
    } else {
        Serial.println(SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE);
        handleEvent(INFO, SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE, SOIL_MOISTURE_INFO);
        startWateringSequenceReturnValue = true; // If soil is dry, start watering sequence
    }

    // Return result
    return startWateringSequenceReturnValue;
}

// Function for activating water pump relay
void DeviceManager::activateWaterPump(bool activate) {
    digitalWrite(DIGITAL_WATER_PUMP_PIN, activate ? HIGH : LOW);
}

// Function for activating soil moisture sensor relay
void DeviceManager::activateSoilMoistureSensor(bool activate) {
    digitalWrite(DIGITAL_SOIL_MOISTURE_SENSOR_PIN, activate ? HIGH : LOW);
    if (activate) {
        delay(1000);
    } else {
        delay(100);
    }
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

// Function that wraps all sensor read related logic
void DeviceManager::handleSensorReadings(unsigned long currentMillis) {
    // Read and send sensor data
    sensorManager.readAndSendTemperature(boardId, networkName);
    sensorManager.readAndSendHumidity(boardId, networkName);
    sensorManager.readAndSendAirPressure(boardId, networkName);
    sensorManager.readAndSendLuminosity(boardId, networkName);
    currentWaterTankLevel = sensorManager.readAndSendWaterTankLevel(boardId, networkName);

    // Reset the timer
    previousSensorMillis = currentMillis; 
}

// Function that handles soil moisture reading related logic
void DeviceManager::handleSoilMoistureReading(unsigned long currentMillis) {
    // Activate soil moisture sensor via relay
    activateSoilMoistureSensor(true);
    delay(1000);
    currentSoilMoisture = sensorManager.readAndSendSoilMoisture(boardId, networkName);
    delay(100);
    // Deactivate soil moisture sensor via relay
    activateSoilMoistureSensor(false);

    startWateringSequence = checkSoilStatus(currentSoilMoisture);
    Serial.println("Start watering sequence:");
    Serial.println(startWateringSequence == 1 ? "true" : "false");
    sensorReadingsDone = true;
    // Reset the timer for the next soil moisture reading
    previousSoilMoistureMillis = currentMillis;
}

void DeviceManager::handleWateringSequence(unsigned long currentMillis) {
    sensorReadingsDone = false;
    startWateringSequence = false;
    // Check if current water tank level is below minimum allowed level
    if (currentWaterTankLevel <= MINIMUM_WATER_TANK_LEVEL) {
        Serial.println("Activating water pump.");
        // Activate water pump via relay
        activateWaterPump(true);
        waterPumpActivatedMillis = currentMillis;
        waterPumpActivated = true;
    } else {
        // Send notification if water tank level is too low
        Serial.println("Water tank level is too low, please refill.");
        sendWaterTankRefillNotification(boardId, networkName);
    }
}

void DeviceManager::handleWaterPumpDeactivation() {
    Serial.println("Stop water pump!");
    waterPumpActivated = false;
    // Deactivate water pump via relay
    activateWaterPump(false);
    sendLatestWateringTime(boardId, networkName);
}

// Main loop function for DeviceManager
void DeviceManager::loop() {
    unsigned long currentMillis = millis(); // Current time in milliseconds since device started

    // Check if its time to read sensors (every 30 seconds)
    if ((currentMillis - previousSensorMillis >= SENSOR_INTERVAL) && isDeviceAuthorized(boardId)) {       
        handleSensorReadings(currentMillis);
    } else {
        // Process created events
        eventModule.loop();
    }

    // Check if its time to read soil moisture (every 12 minutes)
    if ((currentMillis - previousSoilMoistureMillis >= SOIL_MOISTURE_INTERVAL) && isDeviceAuthorized(boardId)) {
        // If the soil moisture interval has passed, read soil moisture
        handleSoilMoistureReading(currentMillis);
    }

    // Check if sensor readings are done and soil status is dry
    if (sensorReadingsDone && startWateringSequence) {
        handleWateringSequence(currentMillis);
    }

    // Check if the water pump has been activated and stop it after the watering sequence
    if (waterPumpActivated && (currentMillis - waterPumpActivatedMillis >= WATERING_SEQUENCE)) {
        handleWaterPumpDeactivation();
    }
}
