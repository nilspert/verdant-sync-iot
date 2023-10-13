/**
 * File: firebase_module.h
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains header file of FirebaseModule.
 * Holds function declarations and global extern for firebase operations.
 */

#ifndef FIREBASE_MODULE_H
#define FIREBASE_MODULE_H

#include <FirebaseESP8266.h>

// Global FirebaseData object for Firebase interactions
extern FirebaseData firebaseData;

// Function for initializing firebase module
void firebaseModuleInit();

// Function for sending data to firebase
bool sendFirebaseData(FirebaseJson json, const char* nodePath);

// Function for checking device authorization
bool isDeviceAuthorized(const String &boardId);

#endif
