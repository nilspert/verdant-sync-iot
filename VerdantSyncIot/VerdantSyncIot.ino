#include <ESP8266WiFi.h>
#include <AESLib.h>
#include <FirebaseESP8266.h>
#include "config.h" // Include configuration file

#define INPUT_BUFFER_LIMIT (128 + 1)

AESLib aesLib;
FirebaseData firebaseData;

// AESLib related variables
unsigned char ciphertext[2*INPUT_BUFFER_LIMIT] = {0}; // THIS IS OUTPUT BUFFER (FOR BASE64-ENCODED ENCRYPTED DATA)
byte aes_key[16]; // AES Encryption Key
byte enc_iv[N_BLOCK]; // General initialization vector
byte enc_iv_1[N_BLOCK];
byte enc_iv_2[N_BLOCK];
byte enc_iv_3[N_BLOCK];
byte enc_iv_4[N_BLOCK];

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
    memcpy(enc_iv_1, ENCRYPTION_SECRET_KEY_IV, sizeof(enc_iv_1));
    memcpy(enc_iv_2, ENCRYPTION_SECRET_KEY_IV, sizeof(enc_iv_2));
    memcpy(enc_iv_3, ENCRYPTION_SECRET_KEY_IV, sizeof(enc_iv_3));
    memcpy(enc_iv_4, ENCRYPTION_SECRET_KEY_IV, sizeof(enc_iv_4));
}

void init_firebase() {
   Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

// Called only once
void setup() {
    Serial.begin(115200);
  
    wifi_init(); // Call WiFi init function
    aes_init(); // Call AESLib init function
    init_firebase(); // Call Firebase init function

    sendBoardInfo();
}

void sendFirebaseData(FirebaseJson json, const char* nodePath) {
    if (Firebase.updateNode(firebaseData, nodePath, json)) {
        Serial.println("Data added to Firebase successfully!");
    } else {
        Serial.print("Error adding data to Firebase: ");
        Serial.println(firebaseData.errorReason());
    }
}

void generateNewIV(byte destinationIV[], const byte sourceIV[]) {
    memcpy(destinationIV, sourceIV, N_BLOCK);
    // Modify the IV here if needed
}

void encryptAndConvertToHex(const char* data, char* encryptedData, byte iv[]) {
    Serial.println("Encrypting and converting to hex string...");
    uint16_t dataLength = strlen(data);
    int cipherLength = aesLib.encrypt((byte*)data, dataLength, (byte*)ciphertext, aes_key, sizeof(aes_key), iv);
    
    for (int i = 0; i < cipherLength; i++) {
        sprintf(encryptedData + (2 * i), "%02X", ciphertext[i]);
    }
}

// Function to gather and encrypt board information
void gatherAndEncryptBoardInfo(char* encryptedFirmwareVersion, char* encryptedBoardName, char* encryptedIpAddress, char* encryptedWifiSSID) {
    encryptAndConvertToHex(FIRMWARE_VERSION, encryptedFirmwareVersion, enc_iv);
    encryptAndConvertToHex(BOARD_NAME, encryptedBoardName, enc_iv_1);
    encryptAndConvertToHex(WiFi.localIP().toString().c_str(), encryptedIpAddress, enc_iv_2);
    encryptAndConvertToHex(WiFi.SSID().c_str(), encryptedWifiSSID, enc_iv_3);
}

// Function to send board information to Firebase
void sendBoardInfo() {
    // Get unique identifier (MAC address) for the board
    String boardId = WiFi.macAddress();

    // Gather and encrypt board information
    char encryptedFirmwareVersion[INPUT_BUFFER_LIMIT] = {0};
    char encryptedBoardName[INPUT_BUFFER_LIMIT] = {0};
    char encryptedIpAddress[INPUT_BUFFER_LIMIT] = {0};
    char encryptedWifiSSID[INPUT_BUFFER_LIMIT] = {0};

    
    gatherAndEncryptBoardInfo(encryptedFirmwareVersion, encryptedBoardName, encryptedIpAddress, encryptedWifiSSID);

    // Create a FirebaseJson object to hold the data
    FirebaseJson json;

    // Add data to the JSON object using identifiers
    json.set(boardId + "/name", encryptedBoardName);
    json.set(boardId + "/firmware", encryptedFirmwareVersion);
    json.set(boardId + "/ip", encryptedIpAddress);
    json.set(boardId + "/ssid", encryptedWifiSSID);

    // Send the data to Firebase
    sendFirebaseData(json, "boards");
}

void sendTemperature() {
  // Get unique identifier (MAC address) for the board
  String boardId = WiFi.macAddress();
  
  float temperature = random(0, 100) + random(0, 99) / 100.0; // Generate a random float between 0 and 100
  char buffer[20]; // Adjust the buffer size as needed
  char encryptedTemperature[INPUT_BUFFER_LIMIT] = {0};
  dtostrf(temperature, 6, 3, buffer);

  // Generate a new IV for each encryption operation
  byte temp_enc_iv[N_BLOCK];
  generateNewIV(temp_enc_iv, enc_iv_4);

  encryptAndConvertToHex(buffer, encryptedTemperature, temp_enc_iv);

  // Create a FirebaseJson object to hold the data
  FirebaseJson json;

  json.set("temperature", encryptedTemperature);

  // Send the data to Firebase
  String nodePath = "boards/" + boardId;
  sendFirebaseData(json, nodePath.c_str());
}

void loop() {
    sendTemperature();

    // Rest of your loop code...

    Serial.println("Looping away...");

    delay(5000);
}
