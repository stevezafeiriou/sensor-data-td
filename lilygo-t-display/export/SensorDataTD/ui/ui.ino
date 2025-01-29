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

// Hardware Definitions
#define DISPLAY_POWER_PIN 15
#define VIBRATION_THRESHOLD 1.65

// LVGL Configuration
static const uint16_t screenWidth = 320;
static const uint16_t screenHeight = 170;
static lv_disp_draw_buf_t draw_buf;
static lv_color_t buf[screenWidth * screenHeight / 10];
TFT_eSPI tft = TFT_eSPI();

// Global Variables
WebSocketsClient webSocket;
Preferences preferences;
bool wifiConnected = false;
String ws_server = "";

// Timing Control
unsigned long last_data = 0;
unsigned long last_display = 0;
const int DATA_INTERVAL = 20;
bool vibration_active = false;

// LVGL Display Flush Function
void my_disp_flush(lv_disp_drv_t *disp, const lv_area_t *area, lv_color_t *color_p) {
    uint32_t w = (area->x2 - area->x1 + 1);
    uint32_t h = (area->y2 - area->y1 + 1);

    tft.startWrite();
    tft.setAddrWindow(area->x1, area->y1, w, h);
    tft.pushColors((uint16_t *)&color_p->full, w * h, true);
    tft.endWrite();

    lv_disp_flush_ready(disp);
}

void initializeWebSocket(String serverIP) {
    ws_server = serverIP;
    webSocket.begin(serverIP, 8080, "/");
    webSocket.setReconnectInterval(5000);
}

void updateUI(float x, float y, float z, bool wifiStatus) {
    // Update sensor values
    if (ui_xValue && ui_yValue && ui_zValue) {
        char buffer[16];
        snprintf(buffer, sizeof(buffer), "%.2f", x);
        lv_label_set_text(ui_xValue, buffer);

        snprintf(buffer, sizeof(buffer), "%.2f", y);
        lv_label_set_text(ui_yValue, buffer);

        snprintf(buffer, sizeof(buffer), "%.2f", z);
        lv_label_set_text(ui_zValue, buffer);
    }

    // Update WiFi status indicator
    lv_obj_set_style_bg_color(ui_wifiConnected,
        wifiStatus ? lv_color_hex(0x006C07) : lv_color_hex(0xFF0000),
        LV_PART_MAIN | LV_STATE_DEFAULT);
}

void updateTitle(const char* text) {
    if (ui_Label6) {
        lv_label_set_text(ui_Label6, text);
        lv_timer_handler();
    }
}

void setup() {
    Serial.begin(115200);

    // Initialize TFT
    tft.begin();
    tft.setRotation(3);

    // Initialize LVGL
    lv_init();
    lv_disp_draw_buf_init(&draw_buf, buf, NULL, screenWidth * screenHeight / 10);

    // Initialize LVGL display driver
    static lv_disp_drv_t disp_drv;
    lv_disp_drv_init(&disp_drv);
    disp_drv.hor_res = screenWidth;
    disp_drv.ver_res = screenHeight;
    disp_drv.flush_cb = my_disp_flush;
    disp_drv.draw_buf = &draw_buf;
    lv_disp_drv_register(&disp_drv);

    // Initialize UI
    ui_init();

    // Initialize hardware
    pinMode(DISPLAY_POWER_PIN, OUTPUT);
    digitalWrite(DISPLAY_POWER_PIN, HIGH);
    Vibration::begin(10);

    if (!MPU6050Sensor::begin(17, 18)) {
        updateTitle("Sensor Error!");
        while (1);
    }

    // Calibrate sensor
    updateTitle("Calibrating...");
    MPU6050Sensor::calibrate();
    updateTitle("Sensor Data TD");

    // Start WiFi Manager
    startWiFiManager();
    preferences.begin("wifi_config", true);
    ws_server = preferences.getString("server_ip", "");
    preferences.end();

    if (wifiConnected && !ws_server.isEmpty()) {
        initializeWebSocket(ws_server);
    }

    // Initialize UI labels
    updateUI(0, 0, 0, wifiConnected);
}

void loop() {
    lv_timer_handler();
    webSocket.loop();

    // Collect sensor data
    if (millis() - last_data >= DATA_INTERVAL) {
        SensorData data = MPU6050Sensor::readData();
        last_data = millis();

        const float DEADZONE = 0.02;
        if (fabs(data.x) < DEADZONE) data.x = 0;
        if (fabs(data.y) < DEADZONE) data.y = 0;
        if (fabs(data.z) < DEADZONE) data.z = 0;

        vibration_active = (fabs(data.x) > VIBRATION_THRESHOLD);

        if (wifiConnected && webSocket.isConnected()) {
            String json = String("{\"x\":") + data.x +
                          ",\"y\":" + data.y +
                          ",\"z\":" + data.z + "}";
            webSocket.sendTXT(json);
        }

        if (millis() - last_display >= 50) {
            updateUI(data.x, data.y, data.z, wifiConnected);
            last_display = millis();
        }
    }

    Vibration::update(vibration_active);
    delay(5);
}
