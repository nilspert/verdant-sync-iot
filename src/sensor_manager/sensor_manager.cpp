/**
 * File: sensor_manager.cpp
 * Author: Joonas Nislin
 * Date: 13.10.2023
 * Description: This file contains implementation of SensorManager.
 * Provides functionality for reading sensors
 * Sensors and actuators:
 * - DHT22 Temperature and humidity sensor
 * - BMP280 Air pressure sensor
 * - Photoresistor luminosity sensor
 * - YL-69 Soil moisture sensor
 * - Water pump
 */

#include "sensor_manager.h"
#include "../globals/globals.h"

// Sensor library variables
Adafruit_BMP280 bmp; // BMP280 sensor
DHT dht(DIGITAL_DHT22_PIN, DHT_TYPE); // DHT22 sensor

// Setup function
void SensorManager::setup() {
    Wire.begin(I2C_D2, I2C_D1); // Initialize I2C communication with specified pins for BMP280
    bmp.begin(0x76); // Initialize BMP280 sensor with specified I2C address (0x76)
    dht.begin(); // Initialize DHT sensor
    pinMode(DIGITAL_HC_SR04_TRIGGER_PIN, OUTPUT);  
    pinMode(DIGITAL_HC_SR04_ECHO_PIN, INPUT); 
}

// Function for reading and sending air pressure data to firebase
void SensorManager::readAndSendAirPressure(String deviceId, String networkName) {
    // Read air pressure from BMP280 and convert to hPa
    float airPressure = bmp.readPressure() / 100.0F;

    // Print air pressure reading
    Serial.print("Air pressure: ");
    Serial.print(airPressure);
    Serial.println(" hPa");

    // Send air pressure data to Firebase
    if (apiManager.encryptAndSendAirPressure(airPressure, deviceId, networkName)) {
        Serial.println("Air pressure data sent successfully.");
    } else {
        Serial.println("Failed to send air pressure data.");
        handleEvent(ERROR, SEND_AIR_PRESSURE_ERROR_MESSAGE, AIR_PRESSURE);
    }
}

// Function for reading and sending temperature data to firebase
void SensorManager::readAndSendTemperature(String deviceId, String networkName) {
    float temperature = dht.readTemperature(); // Read temperature from DHT22

    // Print temperature reading
    Serial.print("Temperature: ");
    Serial.print(temperature);
    Serial.println(" *C");

    // Send temperature data to Firebase
    if (apiManager.encryptAndSendTemperature(temperature, deviceId, networkName)) {
        Serial.println("Temperature data sent successfully.");
    } else {
        Serial.println("Failed to send temperature data.");
        handleEvent(ERROR, SEND_TEMPERATURE_ERROR_MESSAGE, TEMPERATURE);
    }
}

// Function for reading and sending humidity data to firebase
void SensorManager::readAndSendHumidity(String deviceId, String networkName) {
    float humidity = dht.readHumidity(); // Read humidity from DHT22

    // Print humidity reading
    Serial.print("Humidity: ");
    Serial.print(humidity);
    Serial.println(" %");

    // Send humidity data to Firebase
    if (apiManager.encryptAndSendHumidity(humidity, deviceId, networkName)) {
        Serial.println("Humidity data sent successfully.");
    } else {
        Serial.println("Failed to send humidity data.");
        handleEvent(ERROR, SEND_HUMIDITY_ERROR_MESSAGE, HUMIDITY);
    }
}

// Function for reading and sending luminosity data to firebase
void SensorManager::readAndSendLuminosity(String deviceId, String networkName) {
    int luminosity = readPhotoresistor(); // Read luminosity from photoresistor

    // Print luminosity reading
    Serial.print("Luminosity: ");
    Serial.print(luminosity);
    Serial.println(" %");

    // Send luminosity data to Firebase
    if (apiManager.encryptAndSendLuminosity(luminosity, deviceId, networkName)) {
        Serial.println("Luminosity data sent successfully.");
    } else {
        Serial.println("Failed to send luminosity data.");
        handleEvent(ERROR, SEND_LUMINOSITY_ERROR_MESSAGE, LUMINOSITY);
    }
}

// Function for reading photoresistor value
int SensorManager::readPhotoresistor() {
    // Configure Multiplexer IC 74157 to select photoresistor
    // CD74HC4051E_CONTROL_PIN_1 is connected to ground so it stays LOW
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_2, HIGH);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_3, LOW);
    delay(10); // Short delay so that the sensor stabilizes
    int val = analogRead(ANALOG_OUTPUT_PIN); // Read the analog value from sensor
    return val; // Return analog luminosity value
}

// Function for reading and sending soil moisture data to firebase
int SensorManager::readAndSendSoilMoisture(String deviceId, String networkName) {
    // Read soil moisture
    int soilMoisture = readSoilMoistureSensor();

    // Print soil moisture reading
    Serial.print("Soil moisture: ");
    Serial.print(soilMoisture);
    Serial.println(" %");

    // Send soil moisture data to firebase
    if (apiManager.encryptAndSendSoilMoisture(soilMoisture, deviceId, networkName)) {
        Serial.println("Soil moisture data sent successfully.");
    } else {
        Serial.println("Failed to send soil moisture data.");
        handleEvent(ERROR, SEND_SOIL_MOISTURE_ERROR_MESSAGE, SOIL_MOISTURE);
    }

    // Return value of soil moisture
    return soilMoisture;
}

// Function for reading soil moisture sensor value
int SensorManager::readSoilMoistureSensor() {
    // Configure Multiplexer IC 74157 to select soil moisture sensor
    // CD74HC4051E_CONTROL_PIN_1 is connected to ground so it stays LOW
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_2, LOW);
    digitalWrite(DIGITAL_CD74HC4051E_CONTROL_PIN_3, HIGH);
    delay(10); // Short delay so that the sensor stabilizes
    int val = analogRead(ANALOG_OUTPUT_PIN); // Read the analog value from sensor
    return val; // Return analog soil moisture value
}

// Function for reading and sending water tank level data to firebase
float SensorManager::readAndSendWaterTankLevel(String deviceId, String networkName) {
    // Measure water tank level with HC_SR04P
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
    if (distance < 2 || distance > HC_SR04_MAX_DISTANCE_CM) {
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
    if (apiManager.encryptAndSendWaterTankLevel(distance, deviceId, networkName)) {
        Serial.println("Water tank level data sent successfully.");
    } else {
        Serial.println("Failed to send water tank level data.");
        handleEvent(ERROR, SEND_WATER_TANK_LEVEL_ERROR_MESSAGE, WATER_TANK_LEVEL);
    }
    
    // Return value of distance
    return distance;
}
