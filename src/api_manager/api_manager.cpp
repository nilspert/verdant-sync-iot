#include "api_manager.h"
#include "../../config/config.h"
#include "../aes_module/aes_module.h"

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

bool ApiManager::encryptAndSendTemperature(float temperature, const String& boardId) {
    char encryptedTemperature[INPUT_BUFFER_LIMIT] = {0};
    char buffer[20];
    dtostrf(temperature, 6, 2, buffer);

    byte temp_enc_iv[N_BLOCK];
    generateNewIV(temp_enc_iv, enc_ivs[8]);
    encryptAndConvertToHex(buffer, encryptedTemperature, temp_enc_iv);

    FirebaseJson json;
    json.set("temperature", encryptedTemperature);

    String nodePath = "boards/" + boardId;
    return handleApiCall(json, nodePath);
}

bool ApiManager::encryptAndSendSoilMoisture(int soilMoisture, const String& boardId) {
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
    json.set("soil_moisture", encryptedSoilMoisture);

    String nodePath = "boards/" + boardId;
    return handleApiCall(json, nodePath);
}