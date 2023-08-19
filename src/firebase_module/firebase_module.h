#ifndef FIREBASE_MODULE_H
#define FIREBASE_MODULE_H

#include <FirebaseESP8266.h>
#include "../event_module/event_module.h"

extern FirebaseData firebaseData;

void firebaseModuleInit();
bool sendFirebaseData(FirebaseJson json, const char* nodePath);
bool isDeviceAuthorized(const String &boardId, EventModule eventModule);
bool isDeviceAddedToFirebase(const String &boardId, EventModule eventModule);

#endif