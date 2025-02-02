#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WebSocketsClient.h>
#include <Preferences.h>
#include <WiFi.h>
#include "MPU6050Sensor.h"
#include "Vibration.h"
#include "WifiManager.h"

// --------------------------------------------------------------------
// Hardware Definitions
// --------------------------------------------------------------------
#define DISPLAY_POWER_PIN 15
#define VIBRATION_THRESHOLD 1.65  // Vibration threshold

// Hardware definitions for push button and LED
#define BUTTON_PIN 3   // push button pin (using INPUT_PULLUP)
#define LED_PIN    2   // LED pin

TFT_eSPI tft = TFT_eSPI();
WebSocketsClient webSocket;

// --------------------------------------------------------------------
// WiFi Manager Variables
// --------------------------------------------------------------------
Preferences preferences;
bool wifiConnected = false;
String ws_server = "";

// --------------------------------------------------------------------
// Timing variables for sensor & display
// --------------------------------------------------------------------
unsigned long last_data = 0;
unsigned long last_display = 0;
const int DATA_INTERVAL = 20;  // 50Hz data collection

// --------------------------------------------------------------------
// Vibration control
// --------------------------------------------------------------------
bool vibration_active = false;

// --------------------------------------------------------------------
// Device Mode Definitions and Global Variables
// --------------------------------------------------------------------
enum DeviceMode { MODE_A, MODE_B };
DeviceMode currentMode = MODE_A;  // initial mode

// For button double-press detection:
unsigned long lastButtonPressTime = 0;
int buttonPressCount = 0;
bool buttonStateLast = false;  // previous state (for edge detection)

// --------------------------------------------------------------------
// Helper: Convert mode enum to string for display
// --------------------------------------------------------------------
const char* modeToString(DeviceMode mode) {
  return (mode == MODE_A) ? "MODE A" : "MODE B";
}

// --------------------------------------------------------------------
// WebSocket Initialization
// --------------------------------------------------------------------
void initializeWebSocket(String serverIP) {
    ws_server = serverIP;
    webSocket.begin(serverIP, 8080, "/");
    webSocket.setReconnectInterval(5000);
}

// --------------------------------------------------------------------
// Display Update Function
//    Displays sensor data (acceleration and gyro) and the button state.
// --------------------------------------------------------------------
void updateDisplay(const SensorData &data, const String& status, bool btnState) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    
    // Top header with WiFi and mode
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.printf("WiFi: %s\nMode: %s", status.c_str(), modeToString(currentMode));
    
    // Sensor data (Acceleration)
    tft.setTextSize(1);
    tft.setCursor(0, 30);
    tft.printf("Acc:\nX:%.2f\nY:%.2f\nZ:%.2f", data.ax, data.ay, data.az);
    
    // Sensor data (Gyro)
    tft.setCursor(0, 120);
    tft.printf("Gyro:\nX:%.2f\nY:%.2f\nZ:%.2f", data.gx, data.gy, data.gz);
    
    // Button state display
    tft.setCursor(0, 210);
    tft.printf("Button: %s", btnState ? "Pressed" : "Not Pressed");
}

// --------------------------------------------------------------------
// Setup Function
// --------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    pinMode(DISPLAY_POWER_PIN, OUTPUT);
    digitalWrite(DISPLAY_POWER_PIN, HIGH); // Power the display

    // Initialize button and LED pins.
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);
    
    // Initialize modules
    Vibration::begin(10);
    tft.init();
    tft.setRotation(3);
    
    if (!MPU6050Sensor::begin(17, 18)) {
        tft.println("MPU6050 Error!");
        while(1);
    }

    tft.println("Calibrating...");
    MPU6050Sensor::calibrate(); // Calibrate for 3 seconds
    tft.fillScreen(TFT_BLACK);

    // Start WiFi Manager
    startWiFiManager();
    preferences.begin("wifi_config", true);
    ws_server = preferences.getString("server_ip", "");
    preferences.end();
    
    if (wifiConnected && !ws_server.isEmpty()) {
        initializeWebSocket(ws_server);
    }
}

// --------------------------------------------------------------------
// Main Loop
// --------------------------------------------------------------------
void loop() {
    unsigned long currentMillis = millis();

    // Run non-blocking services
    webSocket.loop();
    server.handleClient();
    Vibration::update(vibration_active);

    // -------------------------------
    // Read Button State & Update LED
    // -------------------------------
    // With INPUT_PULLUP, unpressed reads HIGH and pressed reads LOW.
    bool isPressed = (digitalRead(BUTTON_PIN) == LOW);
    digitalWrite(LED_PIN, isPressed ? HIGH : LOW);

    // -------------------------------
    // Button Double-Press Detection to Toggle Modes
    // -------------------------------
    if (!buttonStateLast && isPressed) {  // edge detection: not pressed -> pressed
        if (currentMillis - lastButtonPressTime < 600) {
            buttonPressCount++;
        } else {
            buttonPressCount = 1;
        }
        lastButtonPressTime = currentMillis;
    }
    buttonStateLast = isPressed;
    
    if (buttonPressCount == 2) {
        // Toggle between MODE_A and MODE_B
        currentMode = (currentMode == MODE_A) ? MODE_B : MODE_A;
        buttonPressCount = 0;  // reset the counter
    }
    if (currentMillis - lastButtonPressTime > 600) {
        buttonPressCount = 0;
    }
    
    // -------------------------------
    // Sensor Data Collection (50Hz)
    // -------------------------------
    if (currentMillis - last_data >= DATA_INTERVAL) {
        SensorData data = MPU6050Sensor::readData();
        last_data = currentMillis;

        // Apply a deadzone to acceleration data.
        const float DEADZONE = 0.02;
        if (fabs(data.ax) < DEADZONE) data.ax = 0;
        if (fabs(data.ay) < DEADZONE) data.ay = 0;
        if (fabs(data.az) < DEADZONE) data.az = 0;
        
        // Check vibration threshold (using acceleration).
        vibration_active = (fabs(data.ax) > VIBRATION_THRESHOLD);

        // Compute overall acceleration magnitude.
        float accMag = sqrt(data.ax * data.ax + data.ay * data.ay + data.az * data.az);

        // -------------------------------
        // WebSocket Data Transmission
        // -------------------------------
        // MODE A: Always send data.
        // MODE B: Send data only when the button is pressed.
        bool sendData = false;
        if (currentMode == MODE_A) {
            sendData = true;
        } else if (currentMode == MODE_B) {
            sendData = isPressed;
        }
        
        if (wifiConnected && webSocket.isConnected() && sendData) {
            String json = String("{\"ax\":") + String(data.ax, 2)
                        + ",\"ay\":" + String(data.ay, 2)
                        + ",\"az\":" + String(data.az, 2)
                        + ",\"gx\":" + String(data.gx, 2)
                        + ",\"gy\":" + String(data.gy, 2)
                        + ",\"gz\":" + String(data.gz, 2)
                        + ",\"acc\":" + String(accMag, 2);
            // Always include the button state in the JSON.
            int btnVal = isPressed ? 1 : 0;
            json += ",\"btn\":" + String(btnVal);
            json += "}";
            webSocket.sendTXT(json);
        }
        
        // -------------------------------
        // TFT Display Update
        // -------------------------------
        // MODE A: Always update display with sensor data and button state.
        // MODE B: Update display only when the button is pressed.
        if (currentMode == MODE_A) {
            String status = wifiConnected ? "Connected" : WiFi.softAPIP().toString();
            updateDisplay(data, status, isPressed);
        } else if (currentMode == MODE_B) {
            if (isPressed) {
                String status = wifiConnected ? "Connected" : WiFi.softAPIP().toString();
                updateDisplay(data, status, isPressed);
            } else {
                // Clear the display and show a waiting message.
                tft.fillScreen(TFT_BLACK);
                tft.setTextColor(TFT_WHITE);
                tft.setTextSize(2);
                tft.setCursor(0, 60);
                tft.println("Press Button");
            }
        }
        
        last_display = currentMillis;
    }
}
