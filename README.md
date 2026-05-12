# ScannerDisplay — Metal Scanner Robot UI

A modular, highly optimized UI library designed specifically for a single-axis Metal Scanner Robot. It provides a clean, professional, and readable interface for monitoring real-time telemetry on an ESP32 using an ILI9341 TFT display.

## Features

- **High-Performance Rendering**: Built with delta-tracking. The display only redraws pixels when data actually changes, ensuring your robot's main control loop remains lightning fast without screen flickering.
- **Modular & Clean Architecture**: All complex UI logic and TFT drawing commands are completely isolated in `ScannerDisplay.h` and `ScannerDisplay.cpp`. Your main `.ino` sketch stays clean.
- **`VehicleData` Contract**: A simple struct acts as the bridge between your sensors and the screen. Update the struct, hand it to the display, and let the library handle the rest.
- **Dynamic Color-Coding**: Speedometers, battery gauges, and status bars change color automatically based on predefined safety thresholds (e.g., turning orange or red when battery is low or speed is critical).
- **Dummy Data Simulator**: Includes a built-in dummy data generator (`dummy_data.h`) to let you test and validate the UI layout without needing to connect physical sensors.

## Repository Structure

- **`ScannerDisplay.h` & `ScannerDisplay.cpp`**: The core display library files.
- **`USER_GUIDE.md`**: Detailed instructions, API documentation, and code snippets for integrating the display into your project.
- **`trial.ino`**: An example Arduino sketch demonstrating how to initialize the display and feed it data.
- **`dummy_data.h`**: A helper script to simulate vehicle data for testing purposes.

## Getting Started

### Hardware Requirements
- ESP32 Microcontroller
- ILI9341 TFT Display (or any screen supported by the `TFT_eSPI` library)

### Software Dependencies
- Arduino IDE (or PlatformIO)
- [TFT_eSPI Library](https://github.com/Bodmer/TFT_eSPI) (You must configure `User_Setup.h` within this library for your specific TFT pins and driver).

### Quick Setup

1. Copy `ScannerDisplay.h` and `ScannerDisplay.cpp` into your Arduino project folder.
2. Include the header in your main sketch:
   ```cpp
   #include "ScannerDisplay.h"
   ```
3. Create an instance of the display and the data struct:
   ```cpp
   ScannerDisplay display;
   VehicleData vehicle = { 0.0f, 100, ST_OK, "Starting...", 0 };
   ```
4. Initialize it in `setup()` and update it in `loop()`!

For full examples, color/geometry customization, and more details, please read the [User Guide](USER_GUIDE.md).

## Customization

The interface is highly customizable. Open `ScannerDisplay.h` to easily modify:
- **Color Palette**: RGB565 hex codes for backgrounds, accents, and status warnings.
- **Thresholds**: Tweak the battery percentages or speed limits that trigger warning colors.

Layout geometry (like component coordinates and sizes) can be adjusted at the top of `ScannerDisplay.cpp`.
