/**
 * File: aes_module.h
 * Author: Joonas Nislin
 * Date: 1.9.2023
 * Description: This file contains header file of AesModule.
 * Holds function declarations and constants for AES encryption operations.
 */

#ifndef AES_MODULE_H
#define AES_MODULE_H

#include <ESP8266WiFi.h>
#include <AESLib.h>

#define INPUT_BUFFER_LIMIT (128 + 1) // Max size of input data buffer for encryption
#define NUM_IVS 24 // Number of IVs 

// Array of IVs which can be used in other modules
extern byte enc_ivs[NUM_IVS][N_BLOCK];

// Function to initialize the AES module
void aesModuleInit();

// Function to encrypt data and convert it to hexadecimal string
void encryptAndConvertToHex(const char* data, char* encryptedData, byte iv[]);

// Function to generate new iv vector
void generateNewIV(byte destinationIV[], const byte sourceIV[]);

#endif
