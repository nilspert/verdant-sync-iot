#include "device_manager.h"
#include "../../config/config.h"
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../aes_module/aes_module.h"
#include "../firebase_module/firebase_module.h"
#include "../registration_module/registration_module.h"
#include "../event_module/event_module.h"
#include "../temperature_module/temperature_module.h"
#include "../soil_moisture_module/soil_moisture_module.h"

// Constants and Configuration Settings
const int SERIAL_BAUD_RATE = 115200;
const int LOOP_DELAY = 30000;
const int MOISTURE_SENSOR_PIN = A0;              // Analog pin for moisture sensor readings
const int DIGITAL_MOISTURE_SENSOR_PIN = 10;       // Digital pin for moisture sensor power
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

RegistrationModule registrationModule;
EventModule eventModule;
TemperatureModule temperatureModule;
SoilMoistureModule soilMoistureModule;

unsigned long previousMillis = 0;

void DeviceManager::setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    pinMode(DIGITAL_MOISTURE_SENSOR_PIN, OUTPUT); // Set the digital pin as an input
    digitalWrite(DIGITAL_MOISTURE_SENSOR_PIN, LOW); // Initially keep the sensor OFF
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
    if (registrationModule.encryptAndSendDeviceRegistration(boardId)) {
        handleEvent(INFO, ADD_AUTHORIZED_DEVICE_SUCCESS_MESSAGE);
        if(registrationModule.encryptAndSendBoardInfo(boardId)) {
            handleEvent(INFO, ADD_BOARD_INFO_SUCCESS_MESSAGE);
        } else {
            handleEvent(ERROR, ADD_BOARD_INFO_ERROR_MESSAGE);
        }
    } else {
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

void DeviceManager::loop() {
    unsigned long currentMillis = millis();
    String boardId = getBoardId();

    if ((currentMillis - previousMillis >= LOOP_DELAY) && isDeviceAuthorized(boardId)) {        
        Serial.println("Looping away...");

        // Read temperature
        float temperature = random(0, 100) + random(0, 99) / 100.0;
        if (temperatureModule.encryptAndSendTemperature(temperature, boardId)) {
            handleEvent(INFO, SEND_TEMPERATURE_SUCCESS_MESSAGE);
        } else {
            handleEvent(ERROR, SEND_TEMPERATURE_ERROR_MESSAGE);
        }
        
        // Read soil moisture
        int soilMoisture = readSoilMoistureSensor();
        if (soilMoistureModule.encryptAndSendSoilMoisture(soilMoisture, boardId)) {
            handleEvent(INFO, SEND_SOIL_MOISTURE_SUCCESS_MESSAGE);
        } else {
            handleEvent(ERROR, SEND_SOIL_MOISTURE_ERROR_MESSAGE);
        }

        // Determine soil status
        if (soilMoisture < SOIL_WET) {
            Serial.println("Status: Soil is too wet");
        } else if (soilMoisture >= SOIL_WET && soilMoisture < SOIL_DRY) {
            Serial.println("Status: Soil moisture is perfect");
        } else {
            Serial.println("Status: Soil is too dry - time to water!");
        }

        previousMillis = currentMillis; // Reset the timer
    }

    eventModule.loop();
}
