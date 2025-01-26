#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WebSocketsClient.h>
#include <Preferences.h>
#include <WiFi.h>
#include "MPU6050Sensor.h"
#include "Vibration.h"
#include "WifiManager.h"

// Hardware Definitions
#define DISPLAY_POWER_PIN 15
#define VIBRATION_THRESHOLD 1.65  // 0.05 movement threshold

TFT_eSPI tft = TFT_eSPI();
WebSocketsClient webSocket;

// Wifi Manager Variables
Preferences preferences;
bool wifiConnected = false;
String ws_server = "";

// Timing
unsigned long last_data = 0;
unsigned long last_display = 0;
const int DATA_INTERVAL = 20;  // 50Hz data collection

// Vibration
bool vibration_active = false;

void initializeWebSocket(String serverIP) {
    ws_server = serverIP;
    webSocket.begin(serverIP, 8080, "/");
    webSocket.setReconnectInterval(5000);
}

void updateDisplay(float x, float y, float z, const String& status) {
    tft.fillScreen(TFT_BLACK);
    tft.setTextColor(TFT_WHITE);
    tft.setTextSize(1);
    tft.setCursor(0, 0);
    tft.printf("WiFi: %s", status.c_str());
    
    tft.setTextSize(2);
    tft.setCursor(0, 30);
    tft.printf("X:%.2f\nY:%.2f\nZ:%.2f", x, y, z);
}

void setup() {
    Serial.begin(115200);
    pinMode(DISPLAY_POWER_PIN, OUTPUT);
    digitalWrite(DISPLAY_POWER_PIN, HIGH);

    // Initialize modules
    Vibration::begin(10);
    tft.init();
    tft.setRotation(3);
    
    if(!MPU6050Sensor::begin(17, 18)) {
        tft.println("MPU6050 Error!");
        while(1);
    }

    tft.println("Calibrating...");
    MPU6050Sensor::calibrate(); // 3-second calibration
    tft.fillScreen(TFT_BLACK);

    // Start WiFi Manager
    startWiFiManager();
    preferences.begin("wifi_config", true);
    ws_server = preferences.getString("server_ip", "");
    preferences.end();
    
    if(wifiConnected && !ws_server.isEmpty()) {
        initializeWebSocket(ws_server);
    }
}

void loop() {
    static unsigned long last_data = 0;
    static unsigned long last_display = 0;
    
    // Non-blocking services
    webSocket.loop();
    server.handleClient();
    Vibration::update(vibration_active);

    // High-speed data collection (50Hz)
    if(millis() - last_data >= DATA_INTERVAL) {
        SensorData data = MPU6050Sensor::readData();
        last_data = millis();

        // Apply deadzone to ignore tiny fluctuations
        const float DEADZONE = 0.02; // Adjust this threshold as needed
        if(fabs(data.x) < DEADZONE) data.x = 0;
        if(fabs(data.y) < DEADZONE) data.y = 0;
        if(fabs(data.z) < DEADZONE) data.z = 0;

        // Check vibration threshold (any axis)
        vibration_active = (fabs(data.x) > VIBRATION_THRESHOLD);

        // Immediate transmission of filtered data
        if(wifiConnected && webSocket.isConnected()) {
            String json = String("{\"x\":") + data.x 
                        + ",\"y\":" + data.y 
                        + ",\"z\":" + data.z + "}";
            webSocket.sendTXT(json);
        }

        // Display update (20Hz) with stabilized values
        if(millis() - last_display >= 50) {
            String status = wifiConnected ? "Connected" : WiFi.softAPIP().toString();
            updateDisplay(data.x, data.y, data.z, status);
            last_display = millis();
        }
    }
}