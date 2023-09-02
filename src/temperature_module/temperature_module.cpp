#include "temperature_module.h"
#include "../../config/config.h"
#include "../aes_module/aes_module.h"
#include "../firebase_module/firebase_module.h"

bool TemperatureModule::encryptAndSendTemperature(float temperature, const String& boardId) {
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
        return true;
    } else {
        Serial.println("Failed to send temperature data.");
        return false;
    }
}
