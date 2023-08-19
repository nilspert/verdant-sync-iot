#ifndef DEVICE_MANAGER_H
#define DEVICE_MANAGER_H

#include "../firebase_module/firebase_module.h"
#include "../event_module/event_module.h"
#include "../temperature_module/temperature_module.h"

class DeviceManager {
public:
    void setup();
    void loop();

private:
    void initModules();
    void registerDeviceForAuthorization();
};

#endif