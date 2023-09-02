#ifndef FIREBASE_MODULE_H
#define FIREBASE_MODULE_H

#include <FirebaseESP8266.h>

extern FirebaseData firebaseData;

void firebaseModuleInit();
bool sendFirebaseData(FirebaseJson json, const char* nodePath);
bool isDeviceAuthorized(const String &boardId);
bool isDeviceAddedToFirebase(const String &boardId);

#endif