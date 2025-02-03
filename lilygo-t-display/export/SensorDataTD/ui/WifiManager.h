#ifndef WIFI_MANAGER_H
#define WIFI_MANAGER_H

#include <WebServer.h>

// Declare the server object as external
extern WebServer server;

// Declare initializeWebSocket so it can be used in WifiManager.cpp
void initializeWebSocket(String serverIP);

// Function prototypes
void startWiFiManager();
void handlePortal();

#endif // WIFI_MANAGER_H
