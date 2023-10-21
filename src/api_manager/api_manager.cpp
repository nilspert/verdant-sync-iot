/**
 * File: api_manager.h
 * Author: Joonas Nislin
 * Date: 9.9.2023
 * Description: This file contains implementation of ApiManager.
 * Provides functionality for calling FirebaseModule which handles API requests.
 */

#include "api_manager.h"
#include "../globals/globals.h"

// Function to set up API call for device and history data
bool ApiManager::setupApiCallWithHistoryData(const String& deviceId, const String& networkName, FirebaseJson json, const String& nodePathKey) {
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted WiFi SSID
    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector

    generateNewIV(temp_enc_iv, enc_ivs[18]); // Generate a new IV for encryption
    encryptAndConvertToHex(networkName.c_str(), encryptedWifiSSID, temp_enc_iv); // Encrypt network name and convert to hex
    String encryptedWifiSSIDString(encryptedWifiSSID); // Create String object to hold the encrypted WiFi SSID data

    String nodePath = "devices/" + deviceId; // Define device node path
    if (handleApiCall(json, nodePath)) {
        // Create a history node path and make another API call to store data to history
        String historyNodePath = "history/" + nodePathKey + "/" + getFormattedDate() + encryptedWifiSSIDString + "/" + deviceId + "/" + getCurrentTimeAsString();
        // Call handleApiCall function and return its result
        return handleApiCall(json, historyNodePath);
    } else {
        return false; // Return false if first handleApiCall fails
    }
}

// Function to send data to firebase
bool ApiManager::handleApiCall(FirebaseJson json, const String& nodePath) {
    // Call sendFireBaseData of firebase_module.cpp
    if (sendFirebaseData(json, nodePath.c_str())) {
        return true; // Return true if request succeeds
    } else {
        return false; // Return false if request fails
    }
}

// Function to handle device registration related data to firebase
bool ApiManager::encryptAndSendDeviceRegistration(const String& deviceId, const String& networkName) {
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted WiFi SSID
    char encryptedDeviceId[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted deviceId
    char encryptedDeviceName[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted device name
    
    encryptAndConvertToHex(networkName.c_str(), encryptedWifiSSID, enc_ivs[5]); // Encrypt network name and convert to hex
    encryptAndConvertToHex(deviceId.c_str(), encryptedDeviceId, enc_ivs[6]); // Encrypt deviceId and convert to hex
    encryptAndConvertToHex(DEVICE_NAME, encryptedDeviceName, enc_ivs[7]); // Encrypt device name and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    // Set device registration related fields to JSON payload with encrypted data
    json.set("/authorized", false);
    json.set("/ssid", encryptedWifiSSID);
    json.set("/macAddress", encryptedDeviceId);
    json.set("/name", encryptedDeviceName);

    // Define node path
    String nodePath = "authorized_devices/" + deviceId;

    // call handleApiCall data function and return its result
    return handleApiCall(json, nodePath);
}

// Function to send device info related data to firebase
bool ApiManager::encryptAndSendDeviceInfo(const String& deviceId, const String& networkName, const String& localIp) {
    char encryptedFirmwareVersion[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted firmware version
    char encryptedDeviceName[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted device name
    char encryptedIpAddress[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted ip address
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted WiFi SSID
    char encryptedDeviceId[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted deviceId

    encryptAndConvertToHex(FIRMWARE_VERSION, encryptedFirmwareVersion, enc_ivs[0]); // Encrypt firmware version and convert to hex
    encryptAndConvertToHex(DEVICE_NAME, encryptedDeviceName, enc_ivs[1]); // Encrypt device name and convert to hex
    encryptAndConvertToHex(localIp.c_str(), encryptedIpAddress, enc_ivs[2]); // Encrypt ip address and convert to hex
    encryptAndConvertToHex(networkName.c_str(), encryptedWifiSSID, enc_ivs[3]); // Encrypt network name and convert to hex
    encryptAndConvertToHex(deviceId.c_str(), encryptedDeviceId, enc_ivs[4]); // Encrypt deviceId and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    // Set device info related fields to JSON payload with encrypted data
    json.set("name", encryptedDeviceName);
    json.set("firmware", encryptedFirmwareVersion);
    json.set("ip", encryptedIpAddress);
    json.set("ssid", encryptedWifiSSID);
    json.set("macAddress", encryptedDeviceId);
    
    // Define node path and call handleApiCall function
    String nodePath = "devices/" + deviceId;
    return handleApiCall(json, nodePath); // Return result of handleApiCall
}

// Function to send temperature data to firebase
bool ApiManager::encryptAndSendTemperature(float temperature, const String& deviceId, const String& networkName) {
    char encryptedTemperature[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted temperature
    char buffer[20]; // Create a character array with a size of 20
    dtostrf(temperature, 6, 2, buffer); // Convert temperature value to string and store it in buffer

    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv, enc_ivs[8]); // Generate a new IV for encryption
    encryptAndConvertToHex(buffer, encryptedTemperature, temp_enc_iv); // Encrypt temperature value from buffer and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(TEMPERATURE_KEY, encryptedTemperature); // Set temperature field to JSON payload with encrypted data

    // call setupApiCallWithHistory data function and return its result
    return setupApiCallWithHistoryData(deviceId, networkName, json, TEMPERATURE_KEY); 
}

// Function to send humidity data to firebase
bool ApiManager::encryptAndSendHumidity(float humidity, const String& deviceId, const String& networkName) {
    char encryptedHumidity[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted humidity
    char buffer[20]; // Create a character array with a size of 20
    dtostrf(humidity, 6, 2, buffer); // Convert humidity value to string and store it in buffer

    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv, enc_ivs[15]); // Generate a new IV for encryption
    encryptAndConvertToHex(buffer, encryptedHumidity, temp_enc_iv); // Encrypt humidity value from buffer and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(HUMIDITY_KEY, encryptedHumidity); // Set humidity field to JSON payload with encrypted data

    // call setupApiCallWithHistory data function and return its result
    return setupApiCallWithHistoryData(deviceId, networkName, json, HUMIDITY_KEY);
}

// Function to send air pressure data to firebase
bool ApiManager::encryptAndSendAirPressure(float airPressure, const String& deviceId, const String& networkName) {
    char encryptedAirPressure[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted air pressure
    char buffer[20]; // Create a character array with a size of 20
    dtostrf(airPressure, 6, 2, buffer); // Convert air pressure value to string and store it in buffer

    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv, enc_ivs[16]);  // Generate a new IV for encryption
    encryptAndConvertToHex(buffer, encryptedAirPressure, temp_enc_iv); // Encrypt air pressure value from buffer and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(AIR_PRESSURE_KEY, encryptedAirPressure);

    // call setupApiCallWithHistory data function and return its result
    return setupApiCallWithHistoryData(deviceId, networkName, json, AIR_PRESSURE_KEY);
}

bool ApiManager::encryptAndSendLuminosity(float luminosity, const String& deviceId, const String& networkName) {
    char encryptedLuminosity[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted luminosity
    char buffer[20]; // Create a character array with a size of 20
    dtostrf(luminosity, 6, 2, buffer); // Convert luminosity value to string and store it in buffer

    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv, enc_ivs[17]);  // Generate a new IV for encryption
    encryptAndConvertToHex(buffer, encryptedLuminosity, temp_enc_iv); // Encrypt luminosity value from buffer and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(LUMINOSITY_KEY, encryptedLuminosity); // Set luminosity field to JSON payload with encrypted data

    // call setupApiCallWithHistory data function and return its result
    return setupApiCallWithHistoryData(deviceId, networkName, json, LUMINOSITY_KEY);
}

bool ApiManager::encryptAndSendSoilMoisture(int soilMoisture, const String& deviceId, const String& networkName) {
    char encryptedSoilMoisture[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted soil moisture
    char buffer[20]; // Create a character array with a size of 20
    dtostrf(soilMoisture, 6, 2, buffer); // Convert soilMoisture value to string and store it in buffer

    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv, enc_ivs[14]);  // Generate a new IV for encryption
    encryptAndConvertToHex(buffer, encryptedSoilMoisture, temp_enc_iv); // Encrypt soil moisture value from buffer and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(SOIL_MOISTURE_KEY, encryptedSoilMoisture); // Set soil moisture field to JSON payload with encrypted data

    // call setupApiCallWithHistory data function and return its result
    return setupApiCallWithHistoryData(deviceId, networkName, json, SOIL_MOISTURE_KEY);
}

bool ApiManager::encryptAndSendWaterTankLevel(float waterTankLevel, const String& deviceId, const String& networkName) {
    char encryptedWaterTankLevel[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted luminosity
    char buffer[20]; // Create a character array with a size of 20
    dtostrf(waterTankLevel, 6, 2, buffer); // Convert water tank level value to string and store it in buffer

    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv, enc_ivs[19]);  // Generate a new IV for encryption
    encryptAndConvertToHex(buffer, encryptedWaterTankLevel, temp_enc_iv); // Encrypt water tank level value from buffer and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(WATER_TANK_LEVEL_KEY, encryptedWaterTankLevel); // Set water tank level field to JSON payload with encrypted data

    // call setupApiCallWithHistory data function and return its result
    return setupApiCallWithHistoryData(deviceId, networkName, json, WATER_TANK_LEVEL_KEY);
}

bool ApiManager::encryptAndSendLatestWateringTime(const String& currentTime, const String& deviceId, const String& networkName) {
    char encryptedCurrentTime[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted current time

    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv, enc_ivs[20]);  // Generate a new IV for encryption
    encryptAndConvertToHex(currentTime.c_str(), encryptedCurrentTime, temp_enc_iv); // Encrypt current time value and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(LATEST_WATERING_TIME_KEY, encryptedCurrentTime); // Set latest watering time field to JSON payload with encrypted data

    // call setupApiCallWithHistory data function and return its result
    return setupApiCallWithHistoryData(deviceId, networkName, json, LATEST_WATERING_TIME_KEY);
}

bool ApiManager::encryptAndSendLatestSensorReadingTime(const String& currentTime, const String& deviceId, const String& networkName) {
    char encryptedCurrentTime[INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted current time

    byte temp_enc_iv[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv, enc_ivs[23]);  // Generate a new IV for encryption
    encryptAndConvertToHex(currentTime.c_str(), encryptedCurrentTime, temp_enc_iv); // Encrypt current time value and convert to hex

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(LATEST_SENSOR_READING_TIME_KEY, encryptedCurrentTime); // Set latest watering time field to JSON payload with encrypted data

    // call setupApiCallWithHistory data function and return its result
    return setupApiCallWithHistoryData(deviceId, networkName, json, LATEST_SENSOR_READING_TIME_KEY);
}

bool ApiManager::encryptAndSendWaterTankRefillNotification(const String& currentTime, const String& deviceId, const String& networkName) {
    char encryptedCurrentTime[INPUT_BUFFER_LIMIT] = {0};  // Create array to store encrypted current time
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};  // Create array to store encrypted WiFi SSID
    byte temp_enc_iv_1[N_BLOCK]; // Create array to store temporary initialization vector
    byte temp_enc_iv_2[N_BLOCK]; // Create array to store temporary initialization vector
    generateNewIV(temp_enc_iv_1, enc_ivs[21]);  // Generate a new IV for encryption
    generateNewIV(temp_enc_iv_2, enc_ivs[22]);  // Generate a new IV for encryption
    encryptAndConvertToHex(currentTime.c_str(), encryptedCurrentTime, temp_enc_iv_1); // Encrypt current time value and convert to hex
    encryptAndConvertToHex(networkName.c_str(), encryptedWifiSSID, temp_enc_iv_2); // Encrypt network name value and convert to hex
    String encryptedWifiSSIDString(encryptedWifiSSID); // Create String object to hold the encrypted WiFi SSID data

    FirebaseJson json; // Create FirebaseJson object to store JSON payload
    json.set(WATER_TANK_REFILL_NOTIFICATION_KEY, encryptedCurrentTime); // Set water tank refill notification field to JSON payload with encrypted data
    json.set("notification_read", false); // Set notification read field to JSON payload with encrypted data

    // Define node path
    String nodePath = "notifications/" + getFormattedDate() + encryptedWifiSSIDString + "/" + deviceId + "/" + getCurrentTimeAsString();

    // call handleApiCall function and return check for its result
    if (handleApiCall(json, nodePath)) {
        return true; // If request succeeds return true
    } else {
        return false; // If request fails return false
    }
}
