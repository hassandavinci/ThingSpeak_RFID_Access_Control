#include <WiFi.h>
#include <ThingSpeak.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>
#include <Wire.h> // Include Wire library for I2C communication
#include <LiquidCrystal_I2C.h> // Include LiquidCrystal_I2C library for I2C LCD
#include <ESP32Servo.h>
Servo doorServo;
const int servoPin = 13; 

// WiFi credentials
const char* ssid = "";
const char* password = "";

// ThingSpeak credentials
const unsigned long channelID = ;//place your channel ID
const char* writeAPIKey = "";
const char* readAPIKey = "";

// Array to store data from ThingSpeak
const int NUM_DATA_POINTS = 100; // Set the number of data points you want to store
String tagIDs[NUM_DATA_POINTS];
String names[NUM_DATA_POINTS];

// Define the start and end bytes for the tag ID
#define START_BYTE 0x02
#define END_BYTE 0x03

// Define the pins for the RDM6300 module
#define RFID_RX_PIN 5  // RX pin of ESP32 connected to TX pin of RDM6300
#define RFID_TX_PIN 4  // TX pin of ESP32 connected to RX pin of RDM6300

// Initialize the LiquidCrystal_I2C library with I2C address and LCD dimensions
LiquidCrystal_I2C lcd(0x27, 16, 2);  // 0x27 is the default I2C address for the LCD

String tagID="o"; // Variable to store the received tag ID

WiFiClient client;

void setup() {
  Serial.begin(9600); // Start serial communication for debugging
  Serial2.begin(9600, SERIAL_8N1, RFID_RX_PIN, RFID_TX_PIN); // Start hardware serial communication with RDM6300 module
  doorServo.attach(servoPin,500,2400);
  // Initialize the LCD display
  lcd.init();
  lcd.backlight();

  // Connect to Wi-Fi
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.println("Connecting to WiFi...");
  }
  Serial.println("Connected to WiFi");
  readDataFromThingSpeak();
  // Initialize ThingSpeak
  ThingSpeak.begin(client);
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("welcome");
  doorServo.write(0);
    
  
}
// Variable to store the last tag ID read
String lastTagID = "";

// Variable to store the last time a tag was read
unsigned long lastTagReadTime = 0;

void loop() {
  // Read characters from the RFID module while data is available
  while (Serial2.available() > 0) {
    char c = Serial2.read(); // Read a character from the RFID module
    
    // Check if the character is the start byte
    if (c == START_BYTE) {
      // Clear the previous tag ID
      tagID = "";
    } 
    // Check if the character is the end byte
    else if (c == END_BYTE) {
      // Print the received tag ID
      Serial.println("Tag ID: " + tagID);
      
      // Check if the tag ID is different from the last one read and if enough time has passed
      if (tagID != lastTagID || millis() - lastTagReadTime > 1000) {
        // Match the tag ID with the IDs fetched from ThingSpeak and display the result on the LCD
        matchAndDisplay(tagID);
        
        // Update the last tag ID and last tag read time
        lastTagID = tagID;
        lastTagReadTime = millis();
      }
    } 
    // Append the character to the tag ID
    else {
      tagID += c;
    }
  }

  delay(100); // Add a small delay to avoid excessive loop iterations
}


void sendToThingSpeak(String data) {
  // Create an array to hold values to send to ThingSpeak
  // Field numbers correspond to fields in your ThingSpeak channel
  // Adjust field numbers as needed
  int fieldNumber = 3; // Field number on ThingSpeak to store the RFID tag ID
  ThingSpeak.writeField(channelID, fieldNumber, data, writeAPIKey);
}

void readDataFromThingSpeak() {
  HTTPClient http;
  String url = "http://api.thingspeak.com/channels/2520220/feeds.json?api_key=" + String(readAPIKey) + "&results=" + String(NUM_DATA_POINTS);
  
  Serial.print("Sending HTTP GET request to: ");
  Serial.println(url);

  http.begin(url);
  int httpResponseCode = http.GET();

  if (httpResponseCode == 200) {
    String payload = http.getString();
    Serial.println("Response payload:");
    Serial.println(payload);

    DynamicJsonDocument doc(2048);
    DeserializationError error = deserializeJson(doc, payload);
    
    if (error) {
      Serial.print("deserializeJson() failed: ");
      Serial.println(error.f_str());
      return;
    }

    // Extract tag IDs and names from the JSON response
    for (int i = 0; i < NUM_DATA_POINTS; i++) {
      tagIDs[i] = doc["feeds"][i]["field1"].as<const char*>();
      names[i] = doc["feeds"][i]["field2"].as<const char*>();
      Serial.print("Tag ID: ");
      Serial.println(tagIDs[i]);
      Serial.print("Name: ");
      Serial.println(names[i]);
    }
  } else {
    Serial.print("Error accessing URL. HTTP response code: ");
    Serial.println(httpResponseCode);
  }

  http.end();
}

void matchAndDisplay(String tagID) {
  Serial.println(tagID);
  // Search for the tag ID in the array fetched from ThingSpeak
  for (int i = 0; i < NUM_DATA_POINTS; i++) {
    Serial.println(tagIDs[i]);
    if (tagID == tagIDs[i]) {
      Serial.println(tagIDs[i]);
      // Tag ID found, display the message on LCD
      
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("Access Granted");
      Serial.println("Acess Granted");
      lcd.setCursor(0, 1);
      lcd.print("Name: ");
      lcd.print(names[i]); // Display the name corresponding to the tag ID
      delay(5000); // Display the message for 5 seconds
      lcd.clear();
      sendToThingSpeak(tagID);
      tagID="o";
      Serial.println(tagID);
       for (int pos = 0; pos <= 90; pos += 1) {
      doorServo.writeMicroseconds(map(pos, 0, 90, 500, 2400));
      delay(15); // Adjust this delay to control the speed
    }

    delay(2000); // Wait for 2 seconds with the door open

    // Gradually move the servo back to the closed position
    for (int pos = 90; pos >= 0; pos -= 1) {
      doorServo.writeMicroseconds(map(pos, 0, 90, 500, 2400));
      delay(15); // Adjust this delay to control the speed
    }

      return;
       // Exit the function after displaying the message
    }
  }
  // Tag ID not found, display appropriate message on LCD
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Access Denied");
  delay(5000); // Display the message for 5 seconds
  lcd.clear(); 
  
      delay(15);
  
}
