/**
 * File: firebase_module.cpp
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains implementation of FirebaseModule.
 * Provides functionality for sending data to firebase and checking device authorization status.
 * Uses FirebaseESP8266 library.
 */

#include "firebase_module.h"
#include "../../config/config.h" // Include configuration file

FirebaseData firebaseData;

// Function for initializing Firebase module
void firebaseModuleInit() {
   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

// Function for sending data specific nodepath in Firebase
bool sendFirebaseData(FirebaseJson json, const char* nodePath) {
    if (Firebase.updateNode(firebaseData, nodePath, json)) {
        return true; // Data sent successfully
    } else {
        return false; // Failed to send data
    }
}

// Function for checking authorization status of the device
bool checkDeviceStatus(const String& deviceId) {
    String nodePath = "authorized_devices/" + deviceId + "/authorized";
    if (Firebase.getBool(firebaseData, nodePath)) {
        bool isAuthorized = firebaseData.to<bool>();
        if (isAuthorized) {
            return true; // Device is authorized
        }
    }
    return false; // Device is not authorized
}

// Function for checking if a device is authorized based on its device ID
bool isDeviceAuthorized(const String& deviceId) {
    return checkDeviceStatus(deviceId);
}
