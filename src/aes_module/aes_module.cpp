/**
 * File: aes_module.cpp
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains implementation of AesModule.
 * Provides functionality for encrypting sensor data in hexadecimal string format.
 * Uses AESLib.h library for encoding operations. 
 */

#include "aes_module.h"
#include "../../config/config.h"

AESLib aesLib;

// AESLib related variables
unsigned char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // Create array to store encrypted data
byte aes_key[16]; // AES Encryption Key
byte enc_ivs[NUM_IVS][N_BLOCK]; // General initialization vectors

// Initialize aesLib
void aesModuleInit() {
    aesLib.set_paddingmode((paddingMode)0); // Set padding mode for AES encryption
    memcpy(aes_key, ENCRYPTION_SECRET_KEY, sizeof(aes_key)); // Copy the encryption key from the configuration
    memcpy(enc_ivs[0], ENCRYPTION_SECRET_KEY_IV, sizeof(enc_ivs[0])); // Copy the initial IV (Initialization Vector)

    // Generate new IVs based on the previous one
    for (int i = 1; i < NUM_IVS; i++) {
      generateNewIV(enc_ivs[i], enc_ivs[i - 1]); 
    }
}

// Encrypt the data and convert it to hexadecimal representation
void encryptAndConvertToHex(const char* data, char* encryptedData, byte iv[]) {
    uint16_t dataLength = strlen(data); // Get the length of the input data
    int cipherLength = aesLib.encrypt((byte*)data, dataLength, (byte*)ciphertext, aes_key, sizeof(aes_key), iv); // Encrypt the data
    
    // Convert the encrypted data to hexadecimal representation
    for (int i = 0; i < cipherLength; i++) {
        sprintf(encryptedData + (2 * i), "%02X", ciphertext[i]);
    }
}

// Generate a new IV (Initialization Vector) based on the previous one
void generateNewIV(byte destinationIV[], const byte sourceIV[]) {
    memcpy(destinationIV, sourceIV, N_BLOCK); // Copy the source IV to the destination IV
}
