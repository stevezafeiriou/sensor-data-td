# Sensor Data Transmission System

This project is an ESP32-based system that collects sensor data from an MPU6050 accelerometer, processes it, and transmits it in real-time via a WebSocket server. The system also includes WiFi configuration capabilities and a visual display for feedback.

## Features

- **MPU6050 Accelerometer Integration**: Reads and processes accelerometer data.
- **Real-Time WebSocket Transmission**: Sends sensor data to a specified WebSocket server.
- **WiFi Manager**: Enables WiFi configuration via a web portal.
- **Onboard TFT Display**: Provides visual feedback for connection status and sensor data.
- **Vibration Detection**: Activates based on accelerometer readings exceeding a configurable threshold.

---

## Prerequisites

### Hardware

- **ESP32 Development Board**
- **MPU6050 Accelerometer Module**
- **TFT Display (SPI-Compatible)**
- **Optional**: External vibration motor (for vibration feedback)

### Software

- **Arduino IDE**
- **Required Libraries**:
  - `TFT_eSPI` (for TFT display)
  - `WebSocketsClient` (for WebSocket communication)
  - `Preferences` (for persistent storage)
  - `Adafruit_MPU6050` (for accelerometer support)
  - `Wire` (for I2C communication)

Install the required libraries via the Arduino Library Manager or using the `Sketch -> Include Library -> Manage Libraries` menu.

---

## Installation

1. Clone or download this repository:

   ```bash
   git clone <repository_url>
   cd <repository_folder>
   ```

2. Open the project files in the Arduino IDE:

   - `SensorDataTD.ino`
   - `MPU6050Sensor.h` / `MPU6050Sensor.cpp`
   - `Vibration.h` / `Vibration.cpp`
   - `WifiManager.h` / `WifiManager.cpp`

3. Configure the TFT display pins in the `TFT_eSPI` library configuration (`User_Setup.h`) to match your hardware.

4. Upload the code to the ESP32 using the Arduino IDE.

---

## Usage

### System Initialization

1. **Power On**:

   - The TFT display initializes and shows the calibration message.
   - The MPU6050 accelerometer undergoes a 3-second calibration.

2. **WiFi Configuration**:
   - If no saved WiFi credentials are available, the ESP32 starts in Access Point mode.
   - Connect to the `SensorDataTD` WiFi network (default password: `12345678`).
   - Navigate to `192.168.4.1` in your browser.
   - Enter the WiFi SSID, password, and WebSocket server IP address in the provided portal.
   - The ESP32 attempts to connect to the WiFi and initialize the WebSocket.

### Data Transmission

- Once connected to WiFi, the ESP32 collects and filters sensor data at a 50Hz frequency.
- Processed data is transmitted to the specified WebSocket server in JSON format:
  ```json
  {
  	"x": 1.23,
  	"y": 4.56,
  	"z": 7.89
  }
  ```

### TFT Display

- Displays the current WiFi status (Connected/Disconnected).
- Shows the filtered accelerometer data (X, Y, Z) in real-time.

### Vibration Feedback

- Activates a vibration motor when the accelerometer detects significant movement (exceeding the threshold of 1.65 g).

---

## Code Overview

### SensorDataTD.ino

- Initializes hardware components (TFT, MPU6050, vibration motor).
- Manages the main application loop:
  - Reads sensor data.
  - Filters and processes accelerometer readings.
  - Updates the TFT display.
  - Handles WebSocket communication.

### MPU6050Sensor.h / MPU6050Sensor.cpp

- Provides an interface for the MPU6050 accelerometer.
- Handles sensor initialization, calibration, and data retrieval.
- Includes a low-pass filter for smoothing accelerometer readings.

### Vibration.h / Vibration.cpp

- Controls the vibration motor state:
  - **ON**: Activates the motor.
  - **OFF**: Deactivates the motor.
  - **PAUSE**: Temporarily pauses the motor.

### WifiManager.h / WifiManager.cpp

- Manages WiFi connections and credentials:
  - Configures the ESP32 as both Access Point (AP) and Station (STA).
  - Provides a web portal for entering WiFi credentials.
  - Saves the credentials and WebSocket server IP in non-volatile memory.

---

## Configuration

### TFT Display

Ensure the pin configurations in `User_Setup.h` of the `TFT_eSPI` library match your ESP32 hardware.

### Vibration Threshold

Modify the vibration threshold in the `SensorDataTD.ino` file:

```cpp
#define VIBRATION_THRESHOLD 1.65
```

### WebSocket Server

Set the WebSocket server IP through the web portal. Alternatively, predefine it in the code:

```cpp
String ws_server = "192.168.1.100";
initializeWebSocket(ws_server);
```

---

## Troubleshooting

1. **TFT Display Issues**:

   - Verify pin connections and library configuration (`User_Setup.h`).

2. **WiFi Not Connecting**:

   - Check the entered SSID and password in the web portal.
   - Ensure the target WiFi network is in range.

3. **No Data Transmission**:

   - Verify the WebSocket server IP.
   - Confirm the WebSocket server is running and accessible.

4. **Vibration Feedback Not Working**:
   - Check the motor wiring.
   - Ensure the threshold value is set appropriately for your use case.

---

## Future Improvements

- Implement secure WebSocket communication (WSS) for better security.
- Add OTA (Over-The-Air) update functionality.
- Enhance error handling for WebSocket and WiFi operations.
- Expand sensor support for additional environmental data.

---

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

---

## Contributions

Contributions are welcome! Please submit issues or pull requests to improve this project.

---

## Contact

For questions or support, please contact [steve@saphirelabs.com](steve@saphirelabs.com).
