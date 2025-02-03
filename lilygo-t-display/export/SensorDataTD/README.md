# User Interface for m+ (ESP32 + MPU6050 Motion Capture)

This project provides a full-featured user interface (UI) for the m+ motion capture system based on an ESP32 and MPU6050. The UI is built using LVGL (Light and Versatile Graphics Library) and is designed to run on the **LilyGo T-Display S3 - ESP32S3 development board**. It features dual screens, a calibration indicator, sensor data display (with one decimal place), mode switching (MODE A and MODE B), and integration with a WiFi Manager that supports both Access Point (AP) and Station (STA) modes.

## Features

- **LVGL-Based UI with Dual Screens:**

  - **Main Screen (ui_main):** Displays sensor data (acceleration and gyro) along with the current WiFi status, mode, and button state.
  - **Press Button Screen (ui_pressButton):** Shown in MODE B when the button is not pressed (prompting the user to press the button to view sensor data).

- **Sensor Data Display:**

  - Reads 3-axis acceleration and 3-axis gyro data from the MPU6050.
  - Values are formatted to one decimal (e.g., 0.0) for clarity.

- **Calibration Indicator:**

  - A dedicated UI element (ui_isCallibrated) shows the calibration status.
  - **During calibration:** It displays a dark green color (hex: `0x006C07`).
  - **After calibration:** It updates to an intense green color (hex: `0x00FF00`).

- **Mode Switching & LED Animations:**

  - **MODE A:** The main screen is always displayed.
  - **MODE B:** The main screen is shown only while the external button is pressed; otherwise, a "Press Button" screen is displayed.
  - A double-press on the external button (GPIO3) toggles between modes.
  - LED animations indicate mode changes (1 blink for MODE A, 2 blinks for MODE B), and the LED follows the button state.

- **WiFi Manager (AP + STA):**

  - Uses the ESP32’s dual-mode capability so that the device creates an Access Point for configuration while also attempting to connect to a saved WiFi network.
  - The integrated web server (via WifiManager.h/cpp) handles WiFi credential configuration and firmware updates.

- **Vibration Feedback:**
  - The system activates a vibration motor when significant motion is detected (based on a configurable threshold).

## Get Started

1. **Environment Setup:**

   - Follow the [LilyGo T-Display S3 Setup Guide](https://stevezafeiriou.com/lilygo-t-display-s3-setup/) to configure the Arduino IDE for the LilyGo T-Display S3 board.
   - Make sure to set up the `TFT_eSPI` library correctly. (No modifications to the library are needed if you follow the guide.)

2. **Open the Project:**

   - Open the `ui.ino` file (this UI version) in Arduino.
   - Ensure your Sketchbook location (via **File > Preferences**) is set to the path of this UI project.

3. **Library and Board Selection:**

   - Verify that all required libraries (LVGL, TFT_eSPI, WebSocketsClient, Preferences, etc.) are installed.
   - Select the appropriate board (LilyGo T-Display S3 / ESP32S3).

4. **Build and Upload:**
   - Build and upload the project to your board.

## Important Notes

- This UI version is tailored for the **LilyGo T-Display S3 - ESP32S3 development board**. You can purchase it from [LilyGo](https://www.lilygo.cc/CjcAky) (affiliate link).
- The UI uses LVGL and was generated with SquareLine Studio. Two screens are used:
  - **Main Screen (ui_main):** Displays sensor data, WiFi status, mode, and button state.
  - **Press Button Screen (ui_pressButton):** Displays a message prompting the user to press the button (used in MODE B when the button is not pressed).
- **Calibration:**  
  The MPU6050 sensor undergoes a 3-second calibration on startup. During calibration, the calibration indicator shows dark green; afterward, it updates to intense green.
- **Mode Switching:**  
  A double press of the external button (GPIO3) toggles between MODE A (always showing sensor data) and MODE B (showing sensor data only when the button is pressed).
- **WiFi Manager:**  
  The project operates in AP + STA mode, enabling configuration via a web portal while attempting to connect to a saved WiFi network.
- **No Touchpad Support:**  
  This project does not include a touchpad driver. If you need touch input, you must add a compatible library and driver.

## LCD Driver & Touchpad Comparison Table

(See the original documentation for a comparison of supported drivers. This project is tested with the built-in TFT display of the LilyGo T-Display S3.)

## Deployment

Once you have built and uploaded the project, your device will:

- Boot up and initialize the sensor (performing calibration).
- Start in AP mode if no WiFi credentials are saved.
- Allow configuration of WiFi and WebSocket server through the web portal.
- Display sensor data on the UI according to the selected mode.
- Use LED animations to indicate mode changes.

## Troubleshooting

- **Calibration Indicator:**  
  If the calibration indicator does not maintain the intense green color after calibration, ensure that the UI update function is not continuously overwriting its style. (In this project, the style is forced to refresh only when needed.)
- **WiFi/AP Issues:**  
  Make sure that `server.handleClient()` is being called in the loop (this is necessary for the WiFi Manager to work properly).
- **LVGL UI Issues:**  
  Verify that your SquareLine Studio–generated UI files (ui.h, ui.c) are in the correct location and that LVGL is initialized properly.

This project is now ready for deployment on your **LilyGo T-Display S3 - ESP32S3** development board!
