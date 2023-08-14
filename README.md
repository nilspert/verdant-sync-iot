# Verdant Sync Iot

This project demonstrates how to send board information, including unique identifiers, to the Firebase Realtime Database using an ESP8266 microcontroller. The goal is to provide a mechanism for tracking and managing multiple ESP8266 boards on a network.

## Features

- Send board information to Firebase Realtime Database.
- Utilize constants for Firebase and Wi-Fi credentials from a configuration file.
- Implement a flag to ensure board information is sent only once during setup.
- Enhance code modularity and organization through modular functions.

## Getting Started

### Prerequisites

- Arduino IDE installed.
- ESP8266 board package installed in Arduino IDE.
- FirebaseESP8266 library installed in Arduino IDE.

### Setup

1. Clone or download this repository to your local machine.

2. Open the Arduino IDE and set up the required libraries as mentioned in the "Prerequisites" section.

3. Copy the `config.h.example` as `config.h`

4. Modify the `config.h` file with your actual Firebase and Wi-Fi credentials.

5. Upload the modified sketch to your ESP8266 board.

6. Monitor the serial output to observe the board information being sent to Firebase.

## Usage

- The board information is sent to the "boards" node in the Firebase Realtime Database.

- Board information includes:
  - Unique MAC address identifier.
  - Board name.
  - Firmware version.
  - IP address.
  - Wi-Fi SSID.

- The `sendBoardInfo()` function handles the process of gathering and sending board information.


## License

This project is licensed under the MIT License - see the [LICENSE](LICENSE) file for details.

---

**Note:** Replace placeholders (e.g., "your-project-id") with actual values.
