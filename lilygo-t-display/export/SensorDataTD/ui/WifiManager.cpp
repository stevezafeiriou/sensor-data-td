#include "WifiManager.h"
#include <WiFi.h>
#include <Preferences.h>

// Define the server object
WebServer server(80); // Start the web server on port 80
extern Preferences preferences; // Ensure preferences is accessible
extern bool wifiConnected;
extern String ws_server;
extern void initializeWebSocket(String serverIP);

void startWiFiManager() {
    // Ensure WiFi is in the correct mode
    WiFi.mode(WIFI_AP_STA);  // Enable both Access Point (AP) and Station (STA)

    // Configure static IP for Soft AP
    IPAddress local_IP(192, 168, 4, 1);
    IPAddress gateway(192, 168, 4, 1);
    IPAddress subnet(255, 255, 255, 0);

    if (!WiFi.softAPConfig(local_IP, gateway, subnet)) {
        Serial.println("Soft AP Config failed!");
        return;
    }

    // Start the Soft AP with SSID and Password
    const char *ssid = "SensorDataTD";
    const char *password = "12345678";

    if (!WiFi.softAP(ssid, password)) {
        Serial.println("Failed to start Soft AP.");
        return;
    }

    // Print Soft AP IP
    Serial.print("Soft AP IP Address: ");
    Serial.println(WiFi.softAPIP());

    // Attempt Station connection
    preferences.begin("wifi_config", true);
    String savedSSID = preferences.getString("ssid", "");
    String savedPassword = preferences.getString("password", "");
    preferences.end();

    if (!savedSSID.isEmpty() && !savedPassword.isEmpty()) {
        WiFi.begin(savedSSID.c_str(), savedPassword.c_str());
        int counter = 0;

        while (WiFi.status() != WL_CONNECTED && counter < 30) {
            delay(1000);
            Serial.println("Attempting to connect as a Station...");
            counter++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            wifiConnected = true;
        } else {
            wifiConnected = false;
            Serial.println("Failed to connect as a Station.");
        }
    }

    // Start the WebServer for AP configuration
    server.on("/", handlePortal);
    server.begin();
 
}

void handlePortal() {
    if (server.method() == HTTP_POST) {
        if (!server.hasArg("ssid") || !server.hasArg("password") || !server.hasArg("server_ip")) {
            server.send(400, "text/html", "<html><body><h1>Error</h1><p>SSID, Password, and Server IP are required.</p></body></html>");
            return;
        }

        String newSSID = server.arg("ssid");
        String newPassword = server.arg("password");
        String newServerIP = server.arg("server_ip");

        // Save credentials
        preferences.begin("wifi_config", false);
        preferences.putString("ssid", newSSID);
        preferences.putString("password", newPassword);
        preferences.putString("server_ip", newServerIP);
        preferences.end();

        WiFi.begin(newSSID.c_str(), newPassword.c_str());
        int counter = 0;

        while (WiFi.status() != WL_CONNECTED && counter < 30) {
            delay(1000);
            counter++;
        }

        if (WiFi.status() == WL_CONNECTED) {
            wifiConnected = true;
            server.send(200, "text/html", 
                "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>WiFi Manager</title></head><body>"
                "<h1>WiFi Connected</h1><p>Your device is now connected to the network.</p></body></html>");
            initializeWebSocket(newServerIP);
        } else {
            wifiConnected = false;
            server.send(200, "text/html", 
                "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>WiFi Manager</title></head><body>"
                "<h1>WiFi Connection Failed</h1><p>Please check your credentials and try again.</p></body></html>");
        }
    } else {
        server.send(200, "text/html", 
            "<!doctype html><html lang='en'><head><meta charset='utf-8'><title>WiFi Manager</title></head><body>"
            "<h1>WiFi Manager</h1><form method='POST'>"
            "<label for='ssid'>SSID:</label><br>"
            "<input type='text' id='ssid' name='ssid'><br><br>"
            "<label for='password'>Password:</label><br>"
            "<input type='password' id='password' name='password'><br><br>"
            "<label for='server_ip'>Server IP:</label><br>"
            "<input type='text' id='server_ip' name='server_ip'><br><br>"
            "<button type='submit'>Save</button></form></body></html>");
    }
}
