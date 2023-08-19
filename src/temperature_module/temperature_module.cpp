#include "temperature_module.h"
#include "../../config/config.h"
#include "../utils/constants.h"
#include "../aes_module/aes_module.h"
#include "../firebase_module/firebase_module.h"
#include "../wifi_module/wifi_module.h"
#include "../time_module/time_module.h"

void TemperatureModule::readAndSendTemperatureData(const String& boardId, EventModule eventModule) {
    float temperature = random(0, 100) + random(0, 99) / 100.0;
    encryptAndSendTemperature(temperature, boardId, eventModule);
}

void TemperatureModule::encryptAndSendTemperature(float temperature, const String& boardId, EventModule eventModule) {
    char encryptedTemperature[INPUT_BUFFER_LIMIT] = {0};
    char buffer[20];
    dtostrf(temperature, 6, 2, buffer);

    byte temp_enc_iv[N_BLOCK];
    generateNewIV(temp_enc_iv, enc_ivs[8]);
    encryptAndConvertToHex(buffer, encryptedTemperature, temp_enc_iv);

    FirebaseJson json;
    json.set("temperature", encryptedTemperature);

    String nodePath = "boards/" + boardId;
    if (sendFirebaseData(json, nodePath.c_str())) {
        Serial.println("Temperature data sent successfully.");
        eventModule.createAndEnqueueEvent(getCurrentTimeAsString(), BOARD_NAME, INFO, DEVICE, SEND_TEMPERATURE_SUCCESS_MESSAGE, getNetworkName());
    } else {
        Serial.println("Failed to send temperature data.");
        eventModule.createAndEnqueueEvent(getCurrentTimeAsString(), BOARD_NAME, ERROR, DEVICE, SEND_TEMPERATURE_ERROR_MESSAGE, getNetworkName());
    }
}
