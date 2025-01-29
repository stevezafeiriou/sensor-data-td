# UI project files using SquareLine Studio with TFT_eSPI

## Get Started

1. Open the `.ino` file in Arduino.
2. Go to **File > Preferences** and set the Sketchbook location to the path of your UI project (where this README is located).
3. Go to `libraries/TFT_eSPI` and open `User_Setup.h` or `User_Setup_Select.h` with a text editor to configure pins for your display.
4. Select your board (install if needed).
5. Build and upload the project.

### Important Notes

- Keep in mind this UI is designed to work with the **LilyGo T-display S3 - ESP32S3 development board**.
- You can purchase it from [this link](https://www.lilygo.cc/CjcAky).
- Before uploading the code, you first need to **set up the environment for the LilyGo T-display S3** by following this guide: [LilyGo T-Display S3 Setup](https://stevezafeiriou.com/lilygo-t-display-s3-setup/).
- **No need to modify the `TFT_eSPI` library**, as the setup in the guide is already configured for this specific UI.

### Touchpad Support

This project does not include a touchpad driver. You can use another Arduino library compatible with your touchpad controller.

## LCD Driver Comparison Table

The following table compares different driver libraries based on the devices they support:

### LCD:

| Model       | lvgl_esp32_drivers | TFT_eSPI | lv_drivers | Arduino-GFX      |
| ----------- | ------------------ | -------- | ---------- | ---------------- |
| FT81x       | \*                 |          |            |                  |
| GC9A01      | \*                 | \*       | \*         | \*               |
| hx8357      | \*                 |          |            | \*(A)            |
| hx8357b/c/d | ?                  | \*       |            | \*(B)            |
| ili9163c    | \*                 | \*(-c?)  |            |                  |
| ili9225     |                    | \*       |            | \*               |
| ili9341     | \*                 | \*       | \*         | \*               |
| ili9481     | \*                 | \*       |            | \*(18bit)        |
| ili9486     | \*                 | \*       |            | \*(normal,18bit) |
| ili9488     | \*                 | \*       |            | \*(normal,18bit) |
| R61581      |                    | \*       | \*         |                  |
| RM68120     |                    | \*       |            |                  |
| st7735s     | \*                 | \*(-s?)  |            | \*(-s?)          |
| st7789      | \*                 | \*(\_2?) |            | \*               |
| SSD1963     |                    | \*       | \*         |                  |

### Touch:

| Model           | lvgl_esp32_drivers | TFT_eSPI | lv_drivers |
| --------------- | ------------------ | -------- | ---------- |
| adcraw/AD_touch | \*                 |          | \*         |
| ft6x6           | \*                 |          |            |
| FT81x           | \*                 |          |            |
| gt911           | \*                 |          |            |
| ra8875          | \*                 |          |            |
| stmpe610        | \*                 |          |            |
| xpt2046         | \*                 | \*       | \*         |

_(Note: This list was originally compiled for the Raspberry Pi Pico board template. Supported devices may have changed since then.)_

This project is now ready for deployment on your **LilyGo T-display S3 - ESP32S3** development board!
