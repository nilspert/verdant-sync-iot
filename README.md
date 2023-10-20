# VerdantSync IoT

VerdantSync IoT is an IoT project that automates plant irrigation and monitoring. It uses an ESP8266 microcontroller to send sensor data to Firebase Realtime Database, ensuring efficient plant care and control.

## Features

- **Sensor Data Logging**: Collects data from various sensors and logs it in Firebase.
- **Data Encryption**: Encrypts data sent to Firebase using the AESLib library.
- **Water Pump Control**: Manages a water pump connected to the ESP8266 microcontroller.
- **Android Control App**: Control application can be found at this repository: [VerdantSync](https://github.com/nilspert/verdant-sync)

## Getting Started

### Prerequisites

Before getting started, make sure you have the following prerequisites installed:

- [Arduino IDE](https://www.arduino.cc/en/software)
- [ESP8266WiFi device package](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/ESP8266WiFi.h)
- [AESLib library](https://github.com/suculent/thinx-aes-lib)
- [FirebaseESP8266 library](https://github.com/mobizt/Firebase-ESP8266)
- [BMP280 library](https://github.com/adafruit/Adafruit_BMP280_Library)
- [DHT library](https://github.com/adafruit/DHT-sensor-library)
- [NTPClient library](https://github.com/arduino-libraries/NTPClient)
- [WiFiUdp library](https://github.com/esp8266/Arduino/blob/master/libraries/ESP8266WiFi/src/WiFiUdp.h)
- [Time library](https://github.com/PaulStoffregen/Time)

### Setup

Follow these steps to set up the project:

1. Clone or download this repository to your local machine.

2. Open the Arduino IDE and install the required libraries mentioned in the "Prerequisites" section.

3. Copy the `config.h.example` file as `config.h`.

4. Modify the `config.h` file with your actual Firebase and Wi-Fi credentials.

5. Upload the modified sketch to your ESP8266 device.

6. Monitor the serial output to observe the device information and sensor readings being sent to Firebase.

## Usage

The project has the following functionality:

- **Sensor Data Reading**: Reads and logs temperature, humidity, air pressure, luminosity, and soil moisture using various sensors.

- **Water Pump Control**: Controls a 3.3V water pump based on soil moisture levels.

### Watering Sequence

- The watering sequence begins when the soil moisture level drops below a set threshold (750 / 1024).
- This activates a relay, which in turn controls the water pump to ensure the plant is watered.
- Watering sequence lasts for five seconds. 

## License

This project is open-source and licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
