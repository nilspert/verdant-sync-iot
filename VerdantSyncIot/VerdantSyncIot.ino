#include <ESP8266WiFi.h>
#include <AESLib.h>
#include <FirebaseESP8266.h>
#include "config.h" // Include configuration file

#define INPUT_BUFFER_LIMIT (128 + 1)

AESLib aesLib;
FirebaseData firebaseData;
bool boardInfoSent = false; // Flag to track if board info has been sent

// AESLib related variables
unsigned char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)
byte aes_key[16]; // AES Encryption Key
byte enc_iv[N_BLOCK]; // General initialization vector

// Initialize and connect to WiFi
void wifi_init() {
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.status() != WL_CONNECTED) {
        delay(1000);
        Serial.println("Connecting to WiFi...");
    }
    Serial.println("Connected to WiFi!");
}

// Initialize aesLib
void aes_init() {
    aesLib.set_paddingmode((paddingMode)0);
    memcpy(aes_key, ENCRYPTION_SECRET_KEY, sizeof(aes_key));
    memcpy(enc_iv, ENCRYPTION_SECRET_KEY_IV, sizeof(enc_iv));
}

void encryptAndConvertToHex(const char* data, char* encryptedData) {
    Serial.println("Encrypting and converting to hex string...");
    uint16_t dataLength = strlen(data);
    int cipherLength = aesLib.encrypt((byte*)data, dataLength, (byte*)ciphertext, aes_key, sizeof(aes_key), enc_iv);
    
    for (int i = 0; i < cipherLength; i++) {
        sprintf(encryptedData + (2 * i), "%02X", ciphertext[i]);
    }
}

// Called only once
void setup() {
    Serial.begin(115200);
  
    wifi_init(); // Call WiFi init function
    aes_init(); // Call AESLib init function

    Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

// Function to gather and encrypt board information
void gatherAndEncryptBoardInfo(char* encryptedBoardName, char* encryptedFirmwareVersion, char* encryptedIpAddress, char* encryptedWifiSSID) {
    encryptAndConvertToHex(BOARD_NAME, encryptedBoardName);
    encryptAndConvertToHex(FIRMWARE_VERSION, encryptedFirmwareVersion);
    encryptAndConvertToHex(WiFi.localIP().toString().c_str(), encryptedIpAddress);
    encryptAndConvertToHex(WiFi.SSID().c_str(), encryptedWifiSSID);
}

// Function to send board information to Firebase
void sendBoardInfo() {
    // Get unique identifier (MAC address) for the board
    String boardId = WiFi.macAddress();

    // Gather and encrypt board information
    char encryptedBoardName[INPUT_BUFFER_LIMIT] = {0};
    char encryptedFirmwareVersion[INPUT_BUFFER_LIMIT] = {0};
    char encryptedIpAddress[INPUT_BUFFER_LIMIT] = {0};
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};
    gatherAndEncryptBoardInfo(encryptedBoardName, encryptedFirmwareVersion, encryptedIpAddress, encryptedWifiSSID);

    // Create a FirebaseJson object to hold the data
    FirebaseJson json;

    // Add data to the JSON object using identifiers
    json.set(boardId + "/name", encryptedBoardName);
    json.set(boardId + "/firmware", encryptedFirmwareVersion);
    json.set(boardId + "/ip", encryptedIpAddress);
    json.set(boardId + "/ssid", encryptedWifiSSID);

    // Send the data to Firebase
    if (Firebase.updateNode(firebaseData, "boards", json)) {
        Serial.println("Board info added to Firebase successfully!");
        boardInfoSent = true; // Set the flag to true to prevent sending again
    } else {
        Serial.print("Error adding board info to Firebase: ");
        Serial.println(firebaseData.errorReason());
    }
}

void loop() {
    // Check if board information has been sent
    if (!boardInfoSent) {
        sendBoardInfo();
    }

    // Rest of your loop code...

    Serial.println("Looping away...");

    delay(5000);
}
