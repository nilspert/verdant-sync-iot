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

    // Send the latest watering time to Firebase
    void sendLatestWateringTime(String boardId, String networkName);

    // Send a water tank refill notification to Firebase
    void sendWaterTankRefillNotification(String boardId, String networkName);

    // Check soil moisture status and return boolean to check if watering is needed
    bool checkSoilStatus(int soilMoisture);

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
    const int DIGITAL_SOIL_MOISTURE_SENSOR_PIN = 14;
    const int SOIL_WET_VALUE = 500;
    const int SOIL_DRY_VALUE = 750;
};

#endif
