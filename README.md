# Environmental Monitoring System with SCD30 and Firebase

This project uses an **ESP32 microcontroller**, an **SCD30 sensor**, and **Firebase Realtime Database** to monitor and record environmental data such as temperature, humidity, and CO2 levels. The system synchronizes with an NTP server to provide accurate timestamps for each data entry.

## Features
- **Real-time monitoring** of:
  - Temperature (°C)
  - Humidity (%)
  - CO2 concentration (ppm)
- Automatic **timestamp synchronization** using NTP.
- Data storage in **Firebase Realtime Database** for remote access and analysis.
- JSON-formatted data pushed with unique identifiers.

## Getting Started

### Hardware Requirements
- ESP32 microcontroller
- SCD30 CO2 sensor
- Wi-Fi connection

### Software Requirements
- Arduino IDE
- Libraries:
  - [Firebase ESP Client](https://github.com/mobizt/Firebase-ESP-Client)
  - [Adafruit SCD30](https://github.com/adafruit/Adafruit_SCD30)
  - Arduino core for ESP32

### Installation
1. Clone this repository:
   ```bash
   git clone https://github.com/malikakika/school-air-quality.git
2. Open the project in Arduino IDE.

3. Install the required libraries from the Arduino Library Manager:

Firebase ESP Client
Adafruit SCD30
4.Configure the following details in the code:

#define WIFI_SSID "your_wifi_ssid"
#define WIFI_PASSWORD "your_wifi_password"
#define API_KEY "your_firebase_api_key"
