# Sensor Data Transmission System

This project is an ESP32-based system that collects sensor data from an MPU6050 module, processes it, and transmits it in real-time via a WebSocket server. The system also includes dual WiFi configuration capabilities (AP + STA mode) and provides visual feedback using the **LilyGo T-Display S3**.

## Features

- **MPU6050 Accelerometer & Gyroscope Integration:**  
  Reads and processes both 3-axis acceleration and 3-axis gyro data.

- **Dual Transmission Modes:**:

  - **MODE A**: Always shows all sensor data (acceleration and gyro) plus the current button state on the TFT display.
  - **MODE B**:
    - **Display**: Updates the TFT display with sensor data only when the button is pressed; otherwise, it displays a “Press Button” message.
    - **WebSocket**: Sends sensor data only when the button is pressed.

- **Real-Time WebSocket Transmission:**  
  Transmits sensor data in JSON format to a specified WebSocket server.

- **WiFi Manager (AP + STA mode):**
  Enables WiFi configuration via a web portal. The device operates as both an Access Point (AP) for configuration and as a Station (STA) to connect to your network.

- **Integrated TFT Display:**  
  The LilyGo T-Display S3’s built-in TFT display provides real-time feedback on WiFi status, sensor readings (acceleration and gyro), and button state.

- **Vibration Detection & Feedback:**  
  A vibration motor is activated when the accelerometer detects significant movement (exceeding a configurable threshold).

## Prerequisites

### Hardware

- **LilyGo T-Display S3**
  - Integrated ESP32-S3 and TFT display.
- **MPU6050 Module**
  - Provides both accelerometer and gyroscope data.
- **Push Button & LED**
  - The button is used for mode switching and triggering display/transmission in MODE B.
  - The LED indicates the button state.
- **Vibration Motor (Optional)**
  - For haptic feedback based on movement detection.

### Software

- **Arduino IDE**
- **Required Libraries**:
  - `TFT_eSPI` (for the built-in TFT display)
  - `WebSocketsClient` (for WebSocket communication)
  - `Preferences` (for persistent storage)
  - `Adafruit_MPU6050` (for sensor support)
  - `Wire` (for I2C communication)

### LilyGo T-Display S3 Setup Guide

Follow the guide below to configure the LilyGo T-Display S3 in the Arduino IDE:  
[**LilyGo T-Display S3 - Setup Guide**](https://stevezafeiriou.com/lilygo-t-display-s3-setup/)

## Installation

1. **Clone or Download the Repository:**

   ```bash
   git clone <repository_url>
   cd <repository_folder>
   ```

2. **Open the Project Files in the Arduino IDE:**

   - `SensorDataTD.ino`
   - `MPU6050Sensor.h` / `MPU6050Sensor.cpp`
   - `Vibration.h` / `Vibration.cpp`
   - `WifiManager.h` / `WifiManager.cpp`

3. **Configure the Arduino IDE for the LilyGo T-Display S3**  
   Follow the [setup guide](https://stevezafeiriou.com/lilygo-t-display-s3-setup/) to install the necessary board definitions and libraries.

4. **Upload the Code**  
   Use the Arduino IDE to compile and upload the code to the LilyGo T-Display S3.

## Usage

### System Initialization

1. **Power On:**

   - The TFT display powers up and shows a calibration message.
   - The MPU6050 module undergoes a 3-second calibration (for both acceleration and gyro).

2. **WiFi Configuration:**

   - If no saved WiFi credentials are available, the device starts in Access Point (AP) mode.
   - Connect to the `SensorDataTD` WiFi network (default password: `12345678`).
   - Open your browser and navigate to `192.168.4.1`.
   - Enter your WiFi SSID, password, and WebSocket server IP address in the provided portal.
   - The device will attempt to connect to your network (STA mode) while still hosting the AP for configuration.

### Data Transmission & Display Modes

- **MODE A (Default Mode):**
  - **TFT Display:**  
    Continuously displays all sensor data (3-axis acceleration and 3-axis gyro) along with the current button state.
  - **WebSocket Transmission:**  
    Sends all sensor data in JSON format (including a computed acceleration magnitude and button state) continuously.
- **MODE B:**

  - **TFT Display:**  
    Displays sensor data only when the button is pressed; otherwise, a “Press Button” message is shown.
  - **WebSocket Transmission:**  
    Transmits sensor data only when the button is pressed.

  **Switching Modes:**

  - Double-press the button (two presses within one second) to toggle between MODE A and MODE B.

### Data Format

The sensor data transmitted via WebSocket is sent as a JSON object. For example, in MODE A the JSON data may look like:

```json
{
	"ax": 1.23,
	"ay": 4.56,
	"az": 7.89,
	"gx": 0.12,
	"gy": 0.34,
	"gz": 0.56,
	"acc": 8.9,
	"btn": 1
}
```

In MODE B, data is transmitted only when the button is pressed and does not include the button state.

### Integrated TFT Display

- **Information Shown:**
  - **WiFi Status:** (Connected or the AP IP if not connected)
  - **Mode:** (MODE A or MODE B)
  - **Sensor Data:**
    - Acceleration values (X, Y, Z)
    - Gyroscope values (X, Y, Z)
  - **Button State:** Displayed in MODE A; in MODE B, data is shown only when the button is pressed.

### Vibration Feedback

- The vibration motor is activated when the acceleration on any axis exceeds the defined threshold (`1.65g` by default).
- This threshold can be modified in the source code if needed.

## Code Overview

### SensorDataTD.ino

- **Initialization:**  
  Sets up the TFT display, MPU6050, button, LED, and vibration motor.
- **Main Loop:**
  - Reads sensor data (acceleration and gyro).
  - Applies filtering and processes data.
  - Updates the TFT display based on the current mode.
  - Sends data over WebSocket according to the active mode.
  - Handles button press detection and mode switching.
  - Runs the WiFi manager (AP + STA mode) and web server for configuration.

### MPU6050Sensor.h / MPU6050Sensor.cpp

- **Functionality:**  
  Initializes the MPU6050, performs calibration, and reads both accelerometer and gyroscope data.
- **Data Processing:**  
  Applies a low-pass filter to smooth the sensor data.

### Vibration.h / Vibration.cpp

- **Functionality:**  
  Controls the vibration motor. The motor activates when the sensor data indicates significant movement.

### WifiManager.h / WifiManager.cpp

- **Functionality:**  
  Manages WiFi connections and credentials.
- **Dual Mode Operation:**  
  Configures the ESP32 to operate in both Access Point (AP) and Station (STA) modes.
- **Web Portal:**  
  Provides a web portal for entering WiFi credentials and the WebSocket server IP.

## Configuration

### Vibration Threshold

You can modify the vibration threshold in the `SensorDataTD.ino` file:

```cpp
#define VIBRATION_THRESHOLD 1.65
```

### WebSocket Server

Set the WebSocket server IP address via the web portal. Alternatively, you can predefine it in the code:

```cpp
String ws_server = "192.168.1.100";
initializeWebSocket(ws_server);
```

## Troubleshooting

1. **WiFi Issues:**

   - Verify the entered SSID and password in the web portal.
   - Ensure the target WiFi network is available and within range.
   - Check that the device is in STA mode (if credentials are saved) or connect via AP mode for configuration.

2. **WebSocket Communication:**

   - Confirm the correct WebSocket server IP address.
   - Ensure the WebSocket server is running and accessible.

3. **Sensor Data Problems:**

   - Ensure the MPU6050 module is connected to the correct pins (`SDA -> GPIO17`, `SCL -> GPIO18`).
   - Verify proper power connections to the sensor module.

4. **Vibration Feedback:**
   - Check the wiring of the vibration motor (input pin, e.g., GPIO10, and ground).
   - Adjust the threshold value if the motor is triggering too frequently or not at all.

## Future Improvements

- Implement secure WebSocket (WSS) for encrypted data transmission.
- Add OTA (Over-The-Air) update functionality.
- Enhance error handling for WiFi and sensor operations.
- Expand sensor support to include additional environmental parameters.

## License

This project is licensed under the MIT License. See the [LICENSE](LICENSE) file for details.

## Contributions

Contributions are welcome! Please submit issues or pull requests to help improve this project.

## Contact

For questions or support, please contact [steve@saphirelabs.com](mailto:steve@saphirelabs.com).
