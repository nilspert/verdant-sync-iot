#include "registration_module.h"
#include "../../config/config.h"
#include "../aes_module/aes_module.h"
#include "../firebase_module/firebase_module.h"
#include "../wifi_module/wifi_module.h"

bool RegistrationModule::encryptAndSendDeviceRegistration(const String& boardId) {
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardId[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardName[INPUT_BUFFER_LIMIT] = {0};
    
    encryptAndConvertToHex(getNetworkName().c_str(), encryptedWifiSSID, enc_ivs[5]);
    encryptAndConvertToHex(boardId.c_str(), encryptedBoardId, enc_ivs[6]);
    encryptAndConvertToHex(BOARD_NAME, encryptedBoardName, enc_ivs[7]);

    FirebaseJson json;
    json.set(boardId + "/authorized", false);
    json.set(boardId + "/ssid", encryptedWifiSSID);
    json.set(boardId + "/macAddress", encryptedBoardId);
    json.set(boardId + "/name", encryptedBoardName);

    String nodePath = "authorized_devices/" + boardId + "/authorized";
    if (sendFirebaseData(json, "authorized_devices")) {
        Serial.println("Device successfully registered.");
        return true;
    } else {
        Serial.println("Failed to register device.");
        return false;
    }
}

bool RegistrationModule::encryptAndSendBoardInfo(const String& boardId) {
    char encryptedFirmwareVersion[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardName[INPUT_BUFFER_LIMIT] = {0};
    char encryptedIpAddress[INPUT_BUFFER_LIMIT] = {0};
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardId[INPUT_BUFFER_LIMIT] = {0};

    encryptAndConvertToHex(FIRMWARE_VERSION, encryptedFirmwareVersion, enc_ivs[0]);
    encryptAndConvertToHex(BOARD_NAME, encryptedBoardName, enc_ivs[1]);
    encryptAndConvertToHex(getLocalIpAsString().c_str(), encryptedIpAddress, enc_ivs[2]);
    encryptAndConvertToHex(getNetworkName().c_str(), encryptedWifiSSID, enc_ivs[3]);
    encryptAndConvertToHex(getBoardId().c_str(), encryptedBoardId, enc_ivs[4]);

    FirebaseJson json;
    json.set("name", encryptedBoardName);
    json.set("firmware", encryptedFirmwareVersion);
    json.set("ip", encryptedIpAddress);
    json.set("ssid", encryptedWifiSSID);
    json.set("macAddress", encryptedBoardId);
    
    String nodePath = "boards/" + boardId;
    if (sendFirebaseData(json, nodePath.c_str())) {
        Serial.println("Temperature data sent successfully.");
        return true;
    } else {
        Serial.println("Failed to send temperature data.");
        return false;
    }
}