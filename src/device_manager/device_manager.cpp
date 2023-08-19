#include "device_manager.h"
#include "../../config/config.h"
#include "../utils/constants.h"
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"
#include "../aes_module/aes_module.h"
#include "../registration_module/registration_module.h"

TemperatureModule temperatureModule;
EventModule eventModule;
RegistrationModule registrationModule;

void DeviceManager::setup() {
    Serial.begin(SERIAL_BAUD_RATE);
    initModules();
    updateTime();
    registerDeviceForAuthorization();
}

void DeviceManager::initModules() {
    wifiModuleInit();
    timeModuleInit();
    aesModuleInit();
    firebaseModuleInit();
}

void DeviceManager::registerDeviceForAuthorization() {
    String boardId = getBoardId();
    if (!isDeviceAddedToFirebase(boardId, eventModule)) {
        registrationModule.encryptAndSendDeviceRegistration(boardId, eventModule);
    }
}

void DeviceManager::loop() {
    updateTime();
    String boardId = getBoardId();
    
    if (!isDeviceAuthorized(boardId, eventModule)) {
        eventModule.loop();
        delay(LOOP_DELAY);
        return;
    }
  
    temperatureModule.readAndSendTemperatureData(boardId, eventModule);
    eventModule.loop();

    Serial.println("Looping away...");

    delay(LOOP_DELAY);
}
