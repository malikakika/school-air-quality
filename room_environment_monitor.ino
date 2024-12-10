#include <Arduino.h>
#include <WiFi.h>
#include <Firebase_ESP_Client.h>
#include <Adafruit_SCD30.h> // Library for the SCD30 sensor
#include <time.h>           // Library to handle time using NTP

#include "addons/TokenHelper.h" // Helper for Firebase token handling
#include "addons/RTDBHelper.h"  // Helper for Firebase Realtime Database operations

#define WIFI_SSID "WIFI_SSID"
#define WIFI_PASSWORD "WIFI_PASSWORD"

#define API_KEY "API_KEY"
#define DATABASE_URL "DATABASE_URL"

const char* ntpServer = "pool.ntp.org";
const long gmtOffset_sec = 3600;  // GMT+1 offset
const int daylightOffset_sec = 3600; 

// Firebase and SCD30 objects
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
Adafruit_SCD30 scd30;

unsigned long sendDataPrevMillis = 0;
const unsigned long delayInterval = 60000; // Send data every 1 minute
bool signupOK = false;

// Room identifier
String roomID = "3_2";

// Get a human-readable date string from NTP
String getReadableDate() {
  struct tm timeinfo;
  if (!getLocalTime(&timeinfo)) {
    Serial.println("Error: Unable to sync with NTP.");
    return "1970-01-01 00:00:00"; // Default date if NTP fails
  }
  char buffer[30];
  strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", &timeinfo);
  return String(buffer);
}

void setup() {
  Serial.begin(115200);

  // Connect to Wi-Fi
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  Serial.print("Connecting to Wi-Fi");
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print(".");
    delay(500);
  }
  Serial.println("\nConnected to Wi-Fi!");
  Serial.print("IP Address: ");
  Serial.println(WiFi.localIP());

  // Configure NTP server for time synchronization
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
  Serial.println("Syncing time with NTP...");
  delay(2000); // Allow time for NTP synchronization

  // Set up Firebase configuration
  config.api_key = API_KEY;
  config.database_url = DATABASE_URL;

  // Authenticate with Firebase
  if (Firebase.signUp(&config, &auth, "", "")) {
    Serial.println("Successfully connected to Firebase!");
    signupOK = true;
  } else {
    Serial.printf("Firebase signup error: %s\n", config.signer.signupError.message.c_str());
  }

  config.token_status_callback = tokenStatusCallback; // Firebase token callback
  Firebase.begin(&config, &auth);
  Firebase.reconnectWiFi(true);

  // Initialize the SCD30 sensor
  if (!scd30.begin()) {
    Serial.println("Failed to initialize SCD30. Check connections!");
    while (1); // Stop execution if sensor fails to initialize
  }
  Serial.println("SCD30 initialized successfully.");
}

void loop() {
  // Ensure Firebase and SCD30 are ready before sending data
  if (Firebase.ready() && signupOK && (millis() - sendDataPrevMillis > delayInterval || sendDataPrevMillis == 0)) {
    sendDataPrevMillis = millis();

    // Check if SCD30 data is ready to be read
    if (scd30.dataReady()) {
      if (scd30.read()) {
        float temperature = scd30.temperature;
        float humidity = scd30.relative_humidity;
        float co2 = scd30.CO2;

        Serial.println("Temperature: " + String(temperature) + " °C");
        Serial.println("Humidity: " + String(humidity) + " %");
        Serial.println("CO2: " + String(co2) + " ppm");

        // Retrieve the current date and time
        String readableDate = getReadableDate();

        // Define the Firebase database path
        String path = "/environment/" + roomID;

        // Create a JSON object to store the measurement data
        FirebaseJson json;
        json.set("temperature", temperature);
        json.set("humidity", humidity);
        json.set("co2", co2);
        json.set("date", readableDate);

        // Push data to Firebase with a unique identifier
        if (Firebase.RTDB.pushJSON(&fbdo, path, &json)) {
          Serial.println("Measurement saved successfully. Generated ID: " + fbdo.pushName());
        } else {
          Serial.print("Failed to save data: ");
          Serial.println(fbdo.errorReason());
        }
      } else {
        Serial.println("Failed to read SCD30 data.");
      }
    } else {
      Serial.println("Waiting for SCD30 data...");
    }
  }
}
