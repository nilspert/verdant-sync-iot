# VerdantSync IoT

VerdantSync IoT is an IoT project that automates plant irrigation and monitoring. It uses an ESP8266 microcontroller to collect and send sensor data to Firebase Realtime Database.

## Features

- **Sensor Data Logging**: Collects data from various sensors and sends it to Firebase Realtime Database.
- **Data Encryption**: Encrypts data sent to Firebase using the AESLib library.
- **Water Pump Control**: Manages a water pump connected to the ESP8266 microcontroller.
- **Android Monitoring App**: Monitoring app can be found at this repository: [VerdantSync](https://github.com/nilspert/verdant-sync)

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

### Wiring Diagram

Wiring diagram for the IoT measurement device can be found in [schematics](schematics/VerdantSync_IoT_wiring_diagram.png) folder.

### Components used in IoT Measurement Device

- [KS0367 Keyestudio ESP8266](https://wiki.keyestudio.com/KS0367_keyestudio_ESP8266_WiFi_Board) Wi-Fi Development Board
- Generic Water Pump - Transfers water to the plant from water tank 
- Generic Photoresistor - Reads luminosity
- DHT22 Sensor - Reads air temperature and humidity
- BMP280 Sensor - Reads air pressure
- HC-SR04P Sensor -  Reads water tank level
- YL-69 Sensor - Reads soil moisture
- 3.3V 1 Channel Relay Module x2 - Controls YL-69 and water pump
- CD74HC4051E Multiplexer - used to get readings from YL-69 and photoresistor
- Capacitors 101 C x2
- Resistors (12k x2, 1k x1)
- Diode 1N4001 x3

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

- **Sensor Data Reading**: Reads temperature, humidity, air pressure, luminosity, and soil moisture using various sensors.

- **Water Pump Control**: Controls a 3.3V water pump based on soil moisture levels.

### Watering Sequence

- The watering sequence begins when the soil moisture level drops below a set threshold (750 / 1024).
- This activates a relay, which in turn controls the water pump.
- Watering sequence lasts for twelve seconds. 

## License

This project is open-source and licensed under the MIT License. See the [LICENSE](LICENSE) file for details.
