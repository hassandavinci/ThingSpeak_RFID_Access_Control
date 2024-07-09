# RFID-Based Security System with ThingSpeak

This project implements an RFID-based security system using an ESP32, ThingSpeak, and an LCD display. The system reads RFID tags, checks if the tag is authorized, and then grants or denies access based on the data fetched from ThingSpeak. Additionally, it logs the tag ID to ThingSpeak when access is granted.

## How It Works

1. **Initialization**:
    - The ESP32 connects to a Wi-Fi network using the provided credentials.
    - It initializes the LCD display and the servo motor for the door lock mechanism.
    - It fetches a list of authorized tag IDs and corresponding names from a ThingSpeak channel.

2. **Reading RFID Tags**:
    - The system continuously listens for RFID tags using the RDM6300 RFID module connected to the ESP32.
    - When a tag is scanned, the ESP32 reads the tag ID.

3. **Validating Tag IDs**:
    - The scanned tag ID is compared against the list of authorized tag IDs fetched from ThingSpeak.
    - If the tag ID matches an authorized ID, the system grants access:
        - Displays "Access Granted" and the corresponding name on the LCD.
        - Logs the tag ID to ThingSpeak.
        - Opens the door by moving the servo motor.
        - Keeps the door open for a few seconds, then closes it.

    - If the tag ID does not match any authorized ID, the system denies access:
        - Displays "Access Denied" on the LCD.

## Components Used

- **ESP32**: Microcontroller to manage the system and connect to Wi-Fi.
- **RDM6300 RFID Module**: To read RFID tags.
- **LiquidCrystal_I2C**: To display messages.
- **Servo Motor**: To control the door lock mechanism.
- **ThingSpeak**: Cloud platform to store and retrieve authorized tag IDs.

## Libraries Used

- `WiFi.h`: For Wi-Fi connectivity.
- `ThingSpeak.h`: For interacting with ThingSpeak.
- `HTTPClient.h`: For making HTTP requests.
- `ArduinoJson.h`: For parsing JSON responses.
- `Wire.h`: For I2C communication with the LCD.
- `LiquidCrystal_I2C.h`: For controlling the I2C LCD.
- `ESP32Servo.h`: For controlling the servo motor.

## Code Overview

### Setup

In the `setup()` function, the ESP32:

1. Initializes serial communication for debugging.
2. Initializes hardware serial communication with the RFID module.
3. Attaches the servo motor to the specified pin.
4. Connects to the Wi-Fi network.
5. Fetches authorized tag IDs and names from ThingSpeak.
6. Initializes the LCD display and shows a welcome message.

### Main Loop

In the `loop()` function, the ESP32:

1. Continuously listens for RFID tags.
2. When a tag is detected, reads the tag ID and checks if it is authorized.
3. If authorized, displays a welcome message and opens the door.
4. If not authorized, displays an access denied message.

### Functions

- `readDataFromThingSpeak()`: Fetches authorized tag IDs and names from ThingSpeak.
- `matchAndDisplay(String tagID)`: Matches the scanned tag ID with the authorized IDs and displays the appropriate message on the LCD. Opens the door if access is granted.
- `sendToThingSpeak(String data)`: Sends the scanned tag ID to ThingSpeak when access is granted.
