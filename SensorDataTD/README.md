# Sensor Data Transmission System

This project is an ESP32-based system that collects sensor data from an MPU6050 accelerometer, processes it, and transmits it in real-time via a WebSocket server. The system also includes WiFi configuration capabilities and a visual display for feedback using the **LilyGo T-Display S3**.

## Features

- **MPU6050 Accelerometer Integration**: Reads and processes accelerometer data.
- **Real-Time WebSocket Transmission**: Sends sensor data to a specified WebSocket server.
- **WiFi Manager**: Enables WiFi configuration via a web portal.
- **Integrated TFT Display**: The LilyGo T-Display S3 comes with a built-in TFT display for visual feedback of connection status and sensor data.
- **Vibration Detection**: Activates based on accelerometer readings exceeding a configurable threshold.

## Prerequisites

### Hardware

- **LilyGo T-Display S3**
  - Integrated ESP32-S3 and TFT display
- **MPU6050 Accelerometer Module**
- **Vibration Motor (Optional)**

### Software

- **Arduino IDE**
- **Required Libraries**:
  - `TFT_eSPI` (for the built-in TFT display)
  - `WebSocketsClient` (for WebSocket communication)
  - `Preferences` (for persistent storage)
  - `Adafruit_MPU6050` (for accelerometer support)
  - `Wire` (for I2C communication)

### LilyGo T-Display S3 Setup Guide

Follow the guide below to configure the LilyGo T-Display S3 in the Arduino IDE:  
[**LilyGo T-Display S3 - Setup Guide**](https://stevezafeiriou.com/lilygo-t-display-s3-setup/)

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

3. Follow the [setup guide](https://stevezafeiriou.com/lilygo-t-display-s3-setup/) to configure the Arduino IDE for the LilyGo T-Display S3.

4. Upload the code to the LilyGo T-Display S3 using the Arduino IDE.

## Usage

### System Initialization

1. **Power On**:

   - The integrated TFT display initializes and shows the calibration message.
   - The MPU6050 accelerometer undergoes a 3-second calibration.

2. **WiFi Configuration**:
   - If no saved WiFi credentials are available, the LilyGo T-Display S3 starts in Access Point mode.
   - Connect to the `SensorDataTD` WiFi network (default password: `12345678`).
   - Navigate to `192.168.4.1` in your browser.
   - Enter the WiFi SSID, password, and WebSocket server IP address in the provided portal.
   - The device attempts to connect to the WiFi and initialize the WebSocket.

### Data Transmission

- Once connected to WiFi, the LilyGo T-Display S3 collects and filters sensor data at a 50Hz frequency.
- Processed data is transmitted to the specified WebSocket server in JSON format:
  ```json
  {
  	"x": 1.23,
  	"y": 4.56,
  	"z": 7.89
  }
  ```

### Integrated TFT Display

- Displays the current WiFi status (Connected/Disconnected).
- Shows the filtered accelerometer data (X, Y, Z) in real-time.

### Vibration Feedback

- Activates a vibration motor when the accelerometer detects significant movement (exceeding the threshold of 1.65 g).

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
  - Configures the LilyGo T-Display S3 as both Access Point (AP) and Station (STA).
  - Provides a web portal for entering WiFi credentials.
  - Saves the credentials and WebSocket server IP in non-volatile memory.

## Configuration

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

## Troubleshooting

1. **WiFi Issues**:

   - Check the SSID and password in the WiFi configuration portal.
   - Ensure the target network is available and within range.

2. **Server Not Receiving Data**:

   - Verify the WebSocket server IP is correct.
   - Confirm the WebSocket server is running and accessible.

3. **Accelerometer Data Issues**:

   - Ensure the MPU6050 is connected to the correct pins (`SCL -> GPIO18`, `SDA -> GPIO17`).
   - Verify that the MPU6050 module is powered.

4. **Vibration Feedback Not Working**:
   - Check the motor wiring (`Input -> GPIO10`).
   - Ensure the threshold value is set appropriately in the firmware.

## Future Improvements

- Implement secure WebSocket communication (WSS) for better security.
- Add OTA (Over-The-Air) update functionality.
- Enhance error handling for WebSocket and WiFi operations.
- Expand sensor support for additional environmental data.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributions

Contributions are welcome! Please submit issues or pull requests to improve this project.

## Contact

For questions or support, please contact [steve@saphirelabs.com](mailto:steve@saphirelabs.com).
