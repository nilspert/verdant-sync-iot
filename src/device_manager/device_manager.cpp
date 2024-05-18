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
String deviceId = "";
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

    // Set device related variables
    deviceId = getDeviceId(); // Unique device identifier
    networkName = getNetworkName(); // WiFi SSID

    // Check and register device if not added to Firebase
    if (!isDeviceAuthorized(deviceId)) {
        registerDeviceForAuthorization(deviceId);
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
void DeviceManager::registerDeviceForAuthorization(String deviceId) {
    String localIp = getLocalIpAsString();

    // Register device and send registration data
    if (apiManager.encryptAndSendDeviceRegistration(deviceId, networkName)) {
        Serial.println("Device registered.");

        // Send device info data
        if (apiManager.encryptAndSendDeviceInfo(deviceId, networkName, localIp)) {
            Serial.println("Device info sent.");
        } else {
            Serial.println("Failed to send device info.");
            handleEvent(ERROR, ADD_DEVICE_INFO_ERROR_MESSAGE, DEVICE_INFO);
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
void DeviceManager::sendLatestWateringTime(String deviceId, String networkName) {
    if (apiManager.encryptAndSendLatestWateringTime(getCurrentTimeAsString(), deviceId, networkName)) {
        Serial.println("Watering time sent successfully.");
    } else {
        Serial.println("Failed to send watering time.");
        handleEvent(ERROR, SEND_LATEST_WATERING_TIME_ERROR_MESSAGE, LATEST_WATERING_TIME);
    }
}

// Function for sending latest sensor reading time to firebase
void DeviceManager::sendLatestSensorReadingTime(String deviceId, String networkName) {
    if (apiManager.encryptAndSendLatestSensorReadingTime(getCurrentTimeAsString(), deviceId, networkName)) {
        Serial.println("Sensor reading time sent successfully.");
    } else {
        Serial.println("Failed to send sensor reading time.");
        handleEvent(ERROR, SEND_LATEST_SENSOR_READING_TIME_ERROR_MESSAGE, LATEST_SENSOR_READING_TIME);
    }
}

// Function for sending water tank refill notification to firebase
void DeviceManager::sendWaterTankRefillNotification(String deviceId, String networkName) {
    if (apiManager.encryptAndSendWaterTankRefillNotification(getCurrentTimeAsString(), deviceId, networkName)) {
        Serial.println("Water tank refill notification sent successfully.");
    } else {
        Serial.println("Failed to send water tank refill notification.");
        handleEvent(ERROR, SEND_WATER_TANK_REFILL_NOTIFICATION_ERROR_MESSAGE, WATER_TANK_REFILL_NOTIFICATION);
    }
}

// Function that wraps all sensor read related logic
void DeviceManager::handleSensorReadings(unsigned long currentMillis) {
    // Read and send sensor data
    sensorManager.readAndSendTemperature(deviceId, networkName);
    sensorManager.readAndSendHumidity(deviceId, networkName);
    sensorManager.readAndSendAirPressure(deviceId, networkName);
    sensorManager.readAndSendLuminosity(deviceId, networkName);
    currentWaterTankLevel = sensorManager.readAndSendWaterTankLevel(deviceId, networkName);

    sendLatestSensorReadingTime(deviceId, networkName);
    // Reset the timer
    previousSensorMillis = currentMillis; 
}

// Function that handles soil moisture reading related logic
void DeviceManager::handleSoilMoistureReading(unsigned long currentMillis, bool checkWatering) {
    // Activate soil moisture sensor via relay
    activateSoilMoistureSensor(true);
    delay(1000);
    currentSoilMoisture = sensorManager.readAndSendSoilMoisture(deviceId, networkName);
    delay(100);
    // Deactivate soil moisture sensor via relay
    activateSoilMoistureSensor(false);

    if (checkWatering) {
        checkIfWateringIsNeeded(currentMillis);
    }
}

void DeviceManager::checkIfWateringIsNeeded(unsigned long currentMillis) {
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
        sendWaterTankRefillNotification(deviceId, networkName);
    }
}

void DeviceManager::handleWaterPumpDeactivation(unsigned long currentMillis) {
    Serial.println("Stop water pump!");
    waterPumpActivated = false;
    // Deactivate water pump via relay
    activateWaterPump(false);
    sendLatestWateringTime(deviceId, networkName);
    // Read soil moisture after watering to get the latest readings in app
    handleSoilMoistureReading(currentMillis, false);
}

// Main loop function for DeviceManager
void DeviceManager::loop() {
    unsigned long currentMillis = millis(); // Current time in milliseconds since device started

    // Check if sensor readings are done and soil status is dry
    if (sensorReadingsDone && startWateringSequence) {
        handleWateringSequence(currentMillis);
    } else {
        // Check if its time to read sensors (every 30 seconds)
        if ((currentMillis - previousSensorMillis >= SENSOR_INTERVAL) && isDeviceAuthorized(deviceId)) {       
            handleSensorReadings(currentMillis);
        } else {
            // Process created events
            eventModule.loop();
        }

        // Check if its time to read soil moisture (every 12 minutes)
        if ((currentMillis - previousSoilMoistureMillis >= SOIL_MOISTURE_INTERVAL) && isDeviceAuthorized(deviceId)) {
            // If the soil moisture interval has passed, read soil moisture
            handleSoilMoistureReading(currentMillis, true);
        }
    }

    // Check if the water pump has been activated and stop it after the watering sequence
    if (waterPumpActivated && (currentMillis - waterPumpActivatedMillis >= WATERING_SEQUENCE)) {
        handleWaterPumpDeactivation(currentMillis);
    }
}
