#include "aes_module.h"
#include "../../config/config.h"

AESLib aesLib;

// AESLib related variables
unsigned char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)
byte aes_key[16]; // AES Encryption Key
byte enc_ivs[NUM_IVS][N_BLOCK]; // General initialization vectors

// Initialize aesLib
void aesModuleInit() {
    aesLib.set_paddingmode((paddingMode)0);
    memcpy(aes_key, ENCRYPTION_SECRET_KEY, sizeof(aes_key));
    memcpy(enc_ivs[0], ENCRYPTION_SECRET_KEY_IV, sizeof(enc_ivs[0])); // First IV is the same as before
    for (int i = 1; i < NUM_IVS; i++) {
      generateNewIV(enc_ivs[i], enc_ivs[i - 1]); // Generate new IVs based on the previous one
    }
}

void encryptAndConvertToHex(const char* data, char* encryptedData, byte iv[]) {
    Serial.println("Encrypting and converting to hex string...");
    uint16_t dataLength = strlen(data);
    int cipherLength = aesLib.encrypt((byte*)data, dataLength, (byte*)ciphertext, aes_key, sizeof(aes_key), iv);
    
    for (int i = 0; i < cipherLength; i++) {
        sprintf(encryptedData + (2 * i), "%02X", ciphertext[i]);
    }
}

void generateNewIV(byte destinationIV[], const byte sourceIV[]) {
    memcpy(destinationIV, sourceIV, N_BLOCK);
    // Modify the IV here if needed
}

// Other function definitions...
