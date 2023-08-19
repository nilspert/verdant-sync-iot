#ifndef REGISTRATION_MODULE_H
#define REGISTRATION_MODULE_H

#include <ESP8266WiFi.h>
#include "../event_module/event_module.h"

class RegistrationModule {
public:
    void encryptAndSendDeviceRegistration(const String& boardId, EventModule eventModule);
    void encryptAndSendBoardInfo(const String& boardId, EventModule eventModule);
};

#endif
