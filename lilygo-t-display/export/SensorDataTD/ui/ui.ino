#include <Arduino.h>
#include <TFT_eSPI.h>
#include <WebSocketsClient.h>
#include <Preferences.h>
#include <WiFi.h>
#include <lvgl.h>
#include "MPU6050Sensor.h"
#include "Vibration.h"
#include "WifiManager.h"
#include "ui.h"

// --------------------------------------------------------------------
// Hardware Definitions
// --------------------------------------------------------------------
#define DISPLAY_POWER_PIN 15
#define VIBRATION_THRESHOLD 1.65  // Vibration threshold
#define BUTTON_PIN 3   // External push button (using INPUT_PULLUP)
#define LED_PIN    2   // LED pin

// --------------------------------------------------------------------
// LVGL Display Configuration
// --------------------------------------------------------------------
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];
TFT_eSPI tft = TFT_eSPI();

// --------------------------------------------------------------------
// Global Non-UI Variables
// --------------------------------------------------------------------
WebSocketsClient webSocket;
Preferences preferences;
bool wifiConnected = false;
String ws_server = "";
bool isCalibrated = false;  // Will be set to true after sensor calibration

// --------------------------------------------------------------------
// Timing Control
// --------------------------------------------------------------------
unsigned long last_data = 0;
unsigned long last_display = 0;
const int DATA_INTERVAL = 20;  // 50Hz data collection
bool vibration_active = false;

// --------------------------------------------------------------------
// Mode & Button Variables
// --------------------------------------------------------------------
enum DeviceMode { MODE_A, MODE_B };
DeviceMode currentMode = MODE_A;  // initial mode
unsigned long lastButtonPressTime = 0;
int buttonPressCount = 0;
bool buttonStateLast = false;  // For edge detection on BUTTON_PIN

// --------------------------------------------------------------------
// LED Animation Variables (for mode change)
// --------------------------------------------------------------------
bool modeAnimationActive = false;
unsigned long modeAnimationStart = 0;
int modeAnimationBlinkCount = 0;  // 1 for MODE A, 2 for MODE B
unsigned long lastLEDToggleTime = 0;
bool ledAnimationState = false;

// --------------------------------------------------------------------
// LVGL Display Flush Callback
// --------------------------------------------------------------------
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);
    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();
    lv_disp_flush_ready(disp);
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
// Helper: Convert mode enum to string for display
// --------------------------------------------------------------------
const char* modeToString(DeviceMode mode) {
    return (mode == MODE_A) ? "MODE A" : "MODE B";
}

// --------------------------------------------------------------------
// UI Update Function
//    Updates all UI labels and indicators based on sensor data and state.
// --------------------------------------------------------------------
void updateUIFull(const SensorData &data, bool wifiStatus, bool buttonState, bool calibrated) {
    char buffer[16];
    // Update acceleration values with one decimal for display
    if(ui_axValue) {
        snprintf(buffer, sizeof(buffer), "%.1f", data.ax);
        lv_label_set_text(ui_axValue, buffer);
    }
    if(ui_ayValue) {
        snprintf(buffer, sizeof(buffer), "%.1f", data.ay);
        lv_label_set_text(ui_ayValue, buffer);
    }
    if(ui_azValue) {
        snprintf(buffer, sizeof(buffer), "%.1f", data.az);
        lv_label_set_text(ui_azValue, buffer);
    }
    // Update gyro values with one decimal for display
    if(ui_gxValue) {
        snprintf(buffer, sizeof(buffer), "%.1f", data.gx);
        lv_label_set_text(ui_gxValue, buffer);
    }
    if(ui_gyValue) {
        snprintf(buffer, sizeof(buffer), "%.1f", data.gy);
        lv_label_set_text(ui_gyValue, buffer);
    }
    if(ui_gzValue) {
        snprintf(buffer, sizeof(buffer), "%.1f", data.gz);
        lv_label_set_text(ui_gzValue, buffer);
    }
    // Update WiFi status indicator (ui_wifiConnected): Green if connected, Red otherwise.
    if(ui_wifiConnected) {
        lv_obj_set_style_bg_color(ui_wifiConnected,
            wifiStatus ? lv_color_hex(0x006C07) : lv_color_hex(0xFF0000),
            LV_PART_MAIN | LV_STATE_DEFAULT);
    }
    // Update Calibration Indicator (ui_isCallibrated):
    // It is set to intense green (0x00FF00) if calibrated, dark green (0x006C07) if not.
    if(ui_isCallibrated) {
        lv_obj_set_style_bg_color(ui_isCallibrated,
            calibrated ? lv_color_hex(0x00FF00) : lv_color_hex(0x006C07),
            LV_PART_MAIN | LV_STATE_DEFAULT);
        // Force a style refresh so the color update persists
        lv_obj_refresh_style(ui_isCallibrated, LV_PART_MAIN, (lv_style_prop_t)LV_STYLE_PROP_ALL);
    }
    // Update mode label (ui_mode)
    if(ui_mode) {
        lv_label_set_text(ui_mode, modeToString(currentMode));
    }
    // Update button panel (ui_buttonPressed):
    // Blue if pressed; dark gray (#333547) if not.
    if(ui_buttonPressed) {
        lv_obj_set_style_bg_color(ui_buttonPressed,
            buttonState ? lv_color_hex(0x0000FF) : lv_color_hex(0x333547),
            LV_PART_MAIN | LV_STATE_DEFAULT);
    }
}

// --------------------------------------------------------------------
// Setup Function
// --------------------------------------------------------------------
void setup() {
    Serial.begin(115200);
    // Initialize TFT
    tft.begin();
    tft.setRotation(3);

    // Initialize LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Initialize UI (both screens are set up in ui.h / ui.c)
    ui_init();

    // Initialize hardware
    pinMode(DISPLAY_POWER_PIN, OUTPUT);
    digitalWrite(DISPLAY_POWER_PIN, HIGH);
    Vibration::begin(10);
    pinMode(BUTTON_PIN, INPUT_PULLUP);
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW);

    // Initialize sensor
    if (!MPU6050Sensor::begin(17, 18)) {
        lv_label_set_text(ui_Label6, "Sensor Error!");
        while(1);
    }
    // Set calibration indicator to dark green BEFORE calibration.
    if(ui_isCallibrated) {
        lv_obj_set_style_bg_color(ui_isCallibrated,
            lv_color_hex(0x006C07),  // Dark green during calibration
            LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_invalidate(ui_isCallibrated);
        lv_timer_handler();
    }
    
    // Perform calibration (this blocks for 3 seconds)
    MPU6050Sensor::calibrate();
    isCalibrated = true;
    
    // After calibration, update indicator to intense green.
    if(ui_isCallibrated) {
        lv_obj_set_style_bg_color(ui_isCallibrated,
            lv_color_hex(0x00FF00),  // Intense green after calibration
            LV_PART_MAIN | LV_STATE_DEFAULT);
        lv_obj_invalidate(ui_isCallibrated);
        lv_timer_handler();
        lv_obj_refresh_style(ui_isCallibrated, LV_PART_MAIN, (lv_style_prop_t)LV_STYLE_PROP_ALL);
    }

    // Start WiFi Manager
    startWiFiManager();
    preferences.begin("wifi_config", true);
    ws_server = preferences.getString("server_ip", "");
    preferences.end();
    if (wifiConnected && !ws_server.isEmpty()) {
        initializeWebSocket(ws_server);
    }

    // Initialize UI with starting values (all sensor values 0, button not pressed)
    SensorData initData = {0, 0, 0, 0, 0, 0};
    updateUIFull(initData, wifiConnected, false, isCalibrated);
}

// --------------------------------------------------------------------
// Main Loop
// --------------------------------------------------------------------
void loop() {
    lv_timer_handler();
    webSocket.loop();
    server.handleClient();  // Ensure the WiFi Manager AP works correctly

    // --- Read External Button State (from BUTTON_PIN) ---
    bool isPressed = (digitalRead(BUTTON_PIN) == LOW);

    // --- Mode Switching (double-press detection) ---
    unsigned long currentMillis = millis();
    if (!buttonStateLast && isPressed) {  // Edge detection: not pressed -> pressed
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
        buttonPressCount = 0;
        // Start LED mode animation:
        modeAnimationActive = true;
        modeAnimationStart = currentMillis;
        lastLEDToggleTime = currentMillis;
        ledAnimationState = false;
        // Set blink count: 1 for MODE A, 2 for MODE B.
        modeAnimationBlinkCount = (currentMode == MODE_A) ? 1 : 2;
    }
    if (currentMillis - lastButtonPressTime > 600) {
        buttonPressCount = 0;
    }

    // --- Screen Selection ---
    // In MODE A: Always show the main screen.
    // In MODE B: Show the main screen if the button is pressed; otherwise, show the "Press Button" screen.
    if (currentMode == MODE_A) {
        if (lv_disp_get_scr_act(lv_disp_get_default()) != ui_main) {
            lv_disp_load_scr(ui_main);
        }
    } else {  // MODE B
        if (isPressed) {
            if (lv_disp_get_scr_act(lv_disp_get_default()) != ui_main) {
                lv_disp_load_scr(ui_main);
            }
        } else {
            if (lv_disp_get_scr_act(lv_disp_get_default()) != ui_pressButton) {
                lv_disp_load_scr(ui_pressButton);
            }
        }
    }

    // --- Sensor Data Collection and UI Update ---
    if (millis() - last_data >= DATA_INTERVAL) {
        SensorData data = MPU6050Sensor::readData();
        last_data = millis();

        // Apply deadzone to acceleration values
        const float DEADZONE = 0.02;
        if (fabs(data.ax) < DEADZONE) data.ax = 0;
        if (fabs(data.ay) < DEADZONE) data.ay = 0;
        if (fabs(data.az) < DEADZONE) data.az = 0;
        vibration_active = (fabs(data.ax) > VIBRATION_THRESHOLD);

        // Compute overall acceleration magnitude.
        float accMag = sqrt(data.ax * data.ax + data.ay * data.ay + data.az * data.az);

        // --- WebSocket & Serial Data Transmission ---
        // In MODE A: Always send data.
        // In MODE B: Send data only when the button is pressed.
        bool sendData = false;
        if (currentMode == MODE_A) {
            sendData = true;
        } else if (currentMode == MODE_B) {
            sendData = isPressed;
        }
        // Build JSON string always and print via Serial
        String json = String("{\"ax\":") + String(data.ax, 2) +
                      ",\"ay\":" + String(data.ay, 2) +
                      ",\"az\":" + String(data.az, 2) +
                      ",\"gx\":" + String(data.gx, 2) +
                      ",\"gy\":" + String(data.gy, 2) +
                      ",\"gz\":" + String(data.gz, 2) +
                      ",\"acc\":" + String(accMag, 2) +
                      ",\"btn\":" + String(isPressed ? 1 : 0) +
                      "}";
        // Serial.println(json);  // Always print to Serial
        if (wifiConnected && webSocket.isConnected() && sendData) {
            webSocket.sendTXT(json);
        }

        // --- UI Update ---
        if (millis() - last_display >= 50) {
            updateUIFull(data, wifiConnected, isPressed, isCalibrated);
            last_display = millis();
        }
    }

    // --- LED Update / Animation ---
    if (modeAnimationActive) {
        if (currentMillis - modeAnimationStart < 1000) {
            unsigned long period = 1000UL / (modeAnimationBlinkCount * 2);
            if (currentMillis - lastLEDToggleTime >= period) {
                ledAnimationState = !ledAnimationState;
                digitalWrite(LED_PIN, ledAnimationState ? HIGH : LOW);
                lastLEDToggleTime = currentMillis;
            }
        } else {
            modeAnimationActive = false;
            digitalWrite(LED_PIN, isPressed ? HIGH : LOW);
        }
    } else {
        digitalWrite(LED_PIN, isPressed ? HIGH : LOW);
    }

    Vibration::update(vibration_active);
    delay(5);
}
