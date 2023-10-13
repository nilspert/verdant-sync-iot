/**
 * File: device_manager.h
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains header file of DeviceManager.
 * Holds function declarations and constants for main program operations.
 * - Modules initialization
 * - Device registration
 * - Sensor readings
 * - Water pump control
 */

#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

#define DHT_TYPE DHT22
#define DIGITAL_DHT22_PIN 2

class DeviceManager {
public:
    // Setup function
    void setup();
    // DeviceManager program loop
    void loop();

private:
    // Initialize all modules used by the device
    void initModules();

    // Register the device for authorization on Firebase
    void registerDeviceForAuthorization(String boardId);

    // Read and send temperature data to Firebase
    void readAndSendTemperature(String boardId, String networkName);

    // Read and send humidity data to Firebase
    void readAndSendHumidity(String boardId, String networkName);

    // Read and send air pressure data to Firebase
    void readAndSendAirPressure(String boardId, String networkName);

    // Read and send luminosity data to Firebase
    void readAndSendLuminosity(String boardId, String networkName);

    // Read and send soil moisture data to Firebase and return the moisture level
    int readAndSendSoilMoisture(String boardId, String networkName);

    // Read and send water tank level data to Firebase and return the water tank level
    float readAndSendWaterTankLevel(String boardId, String networkName);

    // Send the latest watering time to Firebase
    void sendLatestWateringTime(String boardId, String networkName);

    // Send a water tank refill notification to Firebase
    void sendWaterTankRefillNotification(String boardId, String networkName);

    // Read the soil moisture sensor and return the moisture level
    int readSoilMoistureSensor();

    // Read the photoresistor and return the light level
    int readPhotoresistor();

    // Check soil moisture status and return boolean to check if watering is needed
    bool checkSoilStatus(int soilMoisture);

    // Handle an event with severity, message and event type
    void handleEvent(const char* severity, const char* message, const char* eventType);

    // Activate or deactivate the water pump
    void activateWaterPump(bool activate);

    // Activate or deactivate soil moisture sensor
    void activateSoilMoistureSensor(bool activate);

    // Constants and Configuration Settings
    const int SERIAL_BAUD_RATE = 115200;
    const int LOOP_DELAY = 30000;
    const int WATERING_SEQUENCE = 5000;
    const int ANALOG_OUTPUT_PIN = A0;
    const int DIGITAL_WATER_PUMP_PIN = 16;
    const int DIGITAL_CD74HC4051E_CONTROL_PIN_2 = 12;
    const int DIGITAL_CD74HC4051E_CONTROL_PIN_3 = 13;
    const int DIGITAL_HC_SR04_TRIGGER_PIN = 0;
    const int DIGITAL_HC_SR04_ECHO_PIN = 15;
    const int DIGITAL_SOIL_MOISTURE_SENSOR_PIN = 14;
    const int I2C_D1 = 5;
    const int I2C_D2 = 4;
    const int SOIL_WET = 500;
    const int SOIL_DRY = 750;
    const int MAX_DISTANCE_CM = 450; 
    const float MINIMUM_WATER_TANK_LEVEL = 12.5;

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
    const char* WATER_TANK_LEVEL = "0x8";
    const char* LATEST_WATERING_TIME = "0x9";
    const char* WATER_TANK_REFILL_NOTIFICATION = "0xA";

    // Event messages
    const char* ADD_BOARD_INFO_ERROR_MESSAGE = "Failed to add board information.";
    const char* ADD_AUTHORIZED_DEVICE_PENDING_MESSAGE = "Device is pending authorization. Data sending disabled.";
    const char* ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE = "Failed to add device to authorized devices";
    const char* SEND_TEMPERATURE_ERROR_MESSAGE = "Failed to send temperature data.";
    const char* SEND_HUMIDITY_ERROR_MESSAGE = "Failed to send humidity data.";
    const char* SEND_AIR_PRESSURE_ERROR_MESSAGE = "Failed to send air pressure data.";
    const char* SEND_SOIL_MOISTURE_ERROR_MESSAGE = "Failed to send soil moisture data.";
    const char* SEND_LUMINOSITY_ERROR_MESSAGE = "Failed to send luminosity data.";
    const char* SEND_WATER_TANK_LEVEL_ERROR_MESSAGE = "Failed to send water tank level data.";
    const char* SEND_LATEST_WATERING_TIME_ERROR_MESSAGE = "Failed to send latest watering time.";
    const char* SEND_WATER_TANK_REFILL_NOTIFICATION_ERROR_MESSAGE = "Failed to send water tank refill notification.";
    const char* SEND_SOIL_MOISTURE_STATUS_WET_MESSAGE = "Status: high soil moisture.";
    const char* SEND_SOIL_MOISTURE_STATUS_OPTIMAL_MESSAGE = "Status: optimal soil moisture.";
    const char* SEND_SOIL_MOISTURE_STATUS_DRY_MESSAGE = "Status: low soil moisture.";
};

#endif
