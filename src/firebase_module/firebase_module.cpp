#include <FirebaseESP8266.h>
#include "../../config/config.h" // Include configuration file

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

bool checkDeviceStatus(const String& boardId) {
    String nodePath = "authorized_devices/" + boardId + "/authorized";
    if (Firebase.getBool(firebaseData, nodePath)) {
        bool isAuthorized = firebaseData.to<bool>();
        if (isAuthorized) {
            Serial.println("Device is authorized.");
            return true;
        }
    }
    Serial.println("Device is not added.");
    return false;
}

bool isDeviceAuthorized(const String& boardId) {
    return checkDeviceStatus(boardId);
}

bool isDeviceAddedToFirebase(const String& boardId) {
    return checkDeviceStatus(boardId);
}

