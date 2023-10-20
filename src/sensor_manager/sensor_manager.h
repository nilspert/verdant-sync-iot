/**
 * File: device_manager.h
 * Author: Joonas Nislin
 * Date: 13.10.2023
 * Description: This file contains header file of SensorManager.
 * Holds function declarations and constants for sensor read operations.
 */

#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <ESP8266WiFi.h>
#include <Wire.h>
#include <Adafruit_Sensor.h>
#include <Adafruit_BMP280.h>
#include <DHT.h>

#define DHT_TYPE DHT22
#define DIGITAL_DHT22_PIN 2

class SensorManager {
public:
    // Setup function
    void setup();

    // Read and send air pressure data to Firebase
    void readAndSendAirPressure(String deviceId, String networkName);

    // Read and send temperature data to Firebase
    void readAndSendTemperature(String deviceId, String networkName);

    // Read and send humidity data to Firebase
    void readAndSendHumidity(String deviceId, String networkName);

    // Read and send luminosity data to Firebase
    void readAndSendLuminosity(String deviceId, String networkName);

    // Read the photoresistor and return the light level
    int readPhotoresistor();

    // Read and send soil moisture data to Firebase and return the moisture level
    int readAndSendSoilMoisture(String deviceId, String networkName);
    
    // Read the soil moisture sensor and return the moisture level
    int readSoilMoistureSensor();

    // Read and send water tank level data to Firebase and return the water tank level
    float readAndSendWaterTankLevel(String deviceId, String networkName);
private:
    // Constants and Configuration Settings
    const int I2C_D1 = 5;
    const int I2C_D2 = 4;
    const int ANALOG_OUTPUT_PIN = A0;
    const int DIGITAL_CD74HC4051E_CONTROL_PIN_2 = 12;
    const int DIGITAL_CD74HC4051E_CONTROL_PIN_3 = 13;
    const int DIGITAL_HC_SR04_TRIGGER_PIN = 0;
    const int DIGITAL_HC_SR04_ECHO_PIN = 15;
    const int HC_SR04_MAX_DISTANCE_CM = 450;
};

#endif