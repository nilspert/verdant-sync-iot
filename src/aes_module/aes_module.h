#ifndef AES_MODULE_H
#define AES_MODULE_H

#include <ESP8266WiFi.h>
#include <AESLib.h>

#define INPUT_BUFFER_LIMIT (128 + 1)
#define NUM_IVS 15 // Number of IVs 

// Declare enc_ivs as extern
extern byte enc_ivs[NUM_IVS][N_BLOCK];

// Function to initialize the AES module
void aesModuleInit();

// Function to encrypt and convert to hex
void encryptAndConvertToHex(const char* data, char* encryptedData, byte iv[]);

// Function to generate new iv vector
void generateNewIV(byte destinationIV[], const byte sourceIV[]);

#endif
