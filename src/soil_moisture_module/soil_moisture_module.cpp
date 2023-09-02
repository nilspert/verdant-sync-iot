#include "soil_moisture_module.h"
#include "../../config/config.h"
#include "../aes_module/aes_module.h"
#include "../firebase_module/firebase_module.h"

bool SoilMoistureModule::encryptAndSendSoilMoisture(int soilMoisture, const String& boardId) {
    // Convert the moistureLevel integer to a string
    String moistureStr = String(soilMoisture);

    // Rest of your code remains the same
    char encryptedSoilMoisture[INPUT_BUFFER_LIMIT] = {0};
    char buffer[20];
    moistureStr.toCharArray(buffer, 20);

    byte temp_enc_iv[N_BLOCK];
    generateNewIV(temp_enc_iv, enc_ivs[14]);
    encryptAndConvertToHex(buffer, encryptedSoilMoisture, temp_enc_iv);

    FirebaseJson json;
    json.set("soil_moisture", encryptedSoilMoisture);

    String nodePath = "boards/" + boardId;
    if (sendFirebaseData(json, nodePath.c_str())) {
        Serial.println("Soil moisture data sent successfully.");
        return true;
    } else {
        Serial.println("Failed to send soil moisture data.");
        return false;
    }
}

