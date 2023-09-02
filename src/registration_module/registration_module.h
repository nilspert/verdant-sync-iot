#ifndef REGISTRATION_MODULE_H
#define REGISTRATION_MODULE_H

#include <ESP8266WiFi.h>

class RegistrationModule {
public:
    bool encryptAndSendDeviceRegistration(const String& boardId);
    bool encryptAndSendBoardInfo(const String& boardId);
};

#endif
