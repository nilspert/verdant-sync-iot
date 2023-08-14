#include <ESP8266WiFi.h>
#include <FirebaseESP8266.h>
#include "config.h" // Include configuration file

FirebaseData firebaseData;
bool boardInfoSent = false; // Flag to track if board info has been sent

void setup() {
  Serial.begin(115200);

  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    delay(1000);
    Serial.println("Connecting to WiFi...");
  }

  Firebase.begin(FIREBASE_HOST, FIREBASE_AUTH);
}

// Function to send board information to Firebase
void sendBoardInfo() {
  // Get unique identifier (MAC address) for the board
  String boardId = WiFi.macAddress();

  // Gather board information from constants in config.h
  String boardName = BOARD_NAME;
  String firmwareVersion = FIRMWARE_VERSION;
  String ipAddress = WiFi.localIP().toString();
  String wifiSSID = WiFi.SSID();

  // Create a FirebaseJson object to hold the data
  FirebaseJson json;

  // Add data to the JSON object using identifiers
  json.set(boardId + "/name", boardName);
  json.set(boardId + "/firmware", firmwareVersion);
  json.set(boardId + "/ip", ipAddress);
  json.set(boardId + "/ssid", wifiSSID);

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
