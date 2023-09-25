#include "api_manager.h"
#include "../../config/config.h"
#include "../aes_module/aes_module.h"

// API node path keys
const char* AIR_PRESSURE_KEY = "air_pressure";
const char* HUMIDITY_KEY = "humidity";
const char* LUMINOSITY_KEY = "luminosity";
const char* SOIL_MOISTURE_KEY = "soil_moisture";
const char* TEMPERATURE_KEY = "temperature";

bool ApiManager::setupApiCallWithHistoryData(const String& boardId, const String& networkName, FirebaseJson json, const String& nodePathKey) {
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};
    byte temp_enc_iv[N_BLOCK]; 
    generateNewIV(temp_enc_iv, enc_ivs[18]);
    encryptAndConvertToHex(networkName.c_str(), encryptedWifiSSID, temp_enc_iv);
    String encryptedWifiSSIDString(encryptedWifiSSID); 

    String nodePath = "boards/" + boardId;
    if (handleApiCall(json, nodePath)) {
        String historyNodePath = "history/" + nodePathKey + "/" + getFormattedDate() + encryptedWifiSSIDString + "/" + boardId + "/" + getCurrentTimeAsString();
        return handleApiCall(json, historyNodePath);
    } else {
        return false;
    }
}

bool ApiManager::handleApiCall(FirebaseJson json, const String& nodePath) {
    if (sendFirebaseData(json, nodePath.c_str())) {
        return true;
    } else {
        return false;
    }
}

bool ApiManager::encryptAndSendDeviceRegistration(const String& boardId, const String& networkName) {
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardId[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardName[INPUT_BUFFER_LIMIT] = {0};
    
    encryptAndConvertToHex(networkName.c_str(), encryptedWifiSSID, enc_ivs[5]);
    encryptAndConvertToHex(boardId.c_str(), encryptedBoardId, enc_ivs[6]);
    encryptAndConvertToHex(BOARD_NAME, encryptedBoardName, enc_ivs[7]);

    FirebaseJson json;
    json.set(boardId + "/authorized", false);
    json.set(boardId + "/ssid", encryptedWifiSSID);
    json.set(boardId + "/macAddress", encryptedBoardId);
    json.set(boardId + "/name", encryptedBoardName);

    String nodePath = "authorized_devices/" + boardId + "/authorized";
    return handleApiCall(json, nodePath);
}

bool ApiManager::encryptAndSendBoardInfo(const String& boardId, const String& networkName, const String& localIp) {
    char encryptedFirmwareVersion[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardName[INPUT_BUFFER_LIMIT] = {0};
    char encryptedIpAddress[INPUT_BUFFER_LIMIT] = {0};
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardId[INPUT_BUFFER_LIMIT] = {0};

    encryptAndConvertToHex(FIRMWARE_VERSION, encryptedFirmwareVersion, enc_ivs[0]);
    encryptAndConvertToHex(BOARD_NAME, encryptedBoardName, enc_ivs[1]);
    encryptAndConvertToHex(localIp.c_str(), encryptedIpAddress, enc_ivs[2]);
    encryptAndConvertToHex(networkName.c_str(), encryptedWifiSSID, enc_ivs[3]);
    encryptAndConvertToHex(boardId.c_str(), encryptedBoardId, enc_ivs[4]);

    FirebaseJson json;
    json.set("name", encryptedBoardName);
    json.set("firmware", encryptedFirmwareVersion);
    json.set("ip", encryptedIpAddress);
    json.set("ssid", encryptedWifiSSID);
    json.set("macAddress", encryptedBoardId);
    
    String nodePath = "boards/" + boardId;
    return handleApiCall(json, nodePath);
}

bool ApiManager::encryptAndSendTemperature(float temperature, const String& boardId, const String& networkName) {
    char encryptedTemperature[INPUT_BUFFER_LIMIT] = {0};
    char buffer[20];
    dtostrf(temperature, 6, 2, buffer);

    byte temp_enc_iv[N_BLOCK];
    generateNewIV(temp_enc_iv, enc_ivs[8]);
    encryptAndConvertToHex(buffer, encryptedTemperature, temp_enc_iv);

    FirebaseJson json;
    json.set(TEMPERATURE_KEY, encryptedTemperature);
    return setupApiCallWithHistoryData(boardId, networkName, json, TEMPERATURE_KEY);
}

bool ApiManager::encryptAndSendHumidity(float humidity, const String& boardId, const String& networkName) {
    char encryptedHumidity[INPUT_BUFFER_LIMIT] = {0};
    char buffer[20];
    dtostrf(humidity, 6, 2, buffer);

    byte temp_enc_iv[N_BLOCK];
    generateNewIV(temp_enc_iv, enc_ivs[15]);
    encryptAndConvertToHex(buffer, encryptedHumidity, temp_enc_iv);

    FirebaseJson json;
    json.set(HUMIDITY_KEY, encryptedHumidity);
    return setupApiCallWithHistoryData(boardId, networkName, json, HUMIDITY_KEY);
}

bool ApiManager::encryptAndSendAirPressure(float airPressure, const String& boardId, const String& networkName) {
    char encryptedAirPressure[INPUT_BUFFER_LIMIT] = {0};
    char buffer[20];
    dtostrf(airPressure, 6, 2, buffer);

    byte temp_enc_iv[N_BLOCK];
    generateNewIV(temp_enc_iv, enc_ivs[16]);
    encryptAndConvertToHex(buffer, encryptedAirPressure, temp_enc_iv);

    FirebaseJson json;
    json.set(AIR_PRESSURE_KEY, encryptedAirPressure);
    return setupApiCallWithHistoryData(boardId, networkName, json, AIR_PRESSURE_KEY);
}

bool ApiManager::encryptAndSendLuminosity(float luminosity, const String& boardId, const String& networkName) {
    char encryptedLuminosity[INPUT_BUFFER_LIMIT] = {0};
    char buffer[20];
    dtostrf(luminosity, 6, 2, buffer);

    byte temp_enc_iv[N_BLOCK];
    generateNewIV(temp_enc_iv, enc_ivs[17]);
    encryptAndConvertToHex(buffer, encryptedLuminosity, temp_enc_iv);

    FirebaseJson json;
    json.set(LUMINOSITY_KEY, encryptedLuminosity);
    return setupApiCallWithHistoryData(boardId, networkName, json, LUMINOSITY_KEY);
}

bool ApiManager::encryptAndSendSoilMoisture(int soilMoisture, const String& boardId, const String& networkName) {
    // Convert the moistureLevel integer to a string
    String moistureStr = String(soilMoisture);

    // Rest of your code remains the same
    char encryptedSoilMoisture[INPUT_BUFFER_LIMIT] = {0};
    char buffer[20];
    moistureStr.toCharArray(buffer, 20);

    byte temp_enc_iv[N_BLOCK];
    generateNewIV(temp_enc_iv, enc_ivs[14]);
    encryptAndConvertToHex(buffer, encryptedSoilMoisture, temp_enc_iv);

    FirebaseJson json;
    json.set(SOIL_MOISTURE_KEY, encryptedSoilMoisture);
    return setupApiCallWithHistoryData(boardId, networkName, json, SOIL_MOISTURE_KEY);
}