#include <FirebaseESP8266.h>
#include "../../config/config.h" // Include configuration file
#include "../utils/constants.h"
#include "../event_module/event_module.h"
#include "../time_module/time_module.h"
#include "../wifi_module/wifi_module.h"

FirebaseData firebaseData;

void firebaseModuleInit() {
   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

bool sendFirebaseData(FirebaseJson json, const char* nodePath) {
    if (Firebase.updateNode(firebaseData, nodePath, json)) {
        return true;
    } else {
        return false;
    }
}

bool checkDeviceStatus(const String& boardId, EventModule eventModule) {
    String nodePath = "authorized_devices/" + boardId + "/authorized";
    if (Firebase.getBool(firebaseData, nodePath)) {
        bool isAuthorized = firebaseData.to<bool>();
        if (isAuthorized) {
            Serial.println("Device already authorized.");
            return true;
        } else {
            Serial.println("Device is pending authorization.");
            eventModule.createAndEnqueueEvent(getCurrentTimeAsString(), BOARD_NAME, ERROR, DEVICE, ADD_AUTHORIZED_DEVICE_ERROR_MESSAGE, getNetworkName());
        }
    }
    Serial.println("Device is not added.");
    return false;
}

bool isDeviceAuthorized(const String& boardId, EventModule eventModule) {
    return checkDeviceStatus(boardId, eventModule);
}

bool isDeviceAddedToFirebase(const String& boardId, EventModule eventModule) {
    return checkDeviceStatus(boardId, eventModule);
}

