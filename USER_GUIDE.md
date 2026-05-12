# ScannerDisplay — User Guide

The `ScannerDisplay` module is a drop-in, highly optimized UI library designed specifically for the Metal Scanner Robot. It handles all the complex TFT drawing, color coding, and geometry in the background, allowing your main robot code to focus purely on sensor logic.

---

## 1. Installation

To use this display in your main robot project, you only need two files:
1. `ScannerDisplay.h`
2. `ScannerDisplay.cpp`

Simply copy these two files into your main Arduino project folder (the same folder as your `.ino` file).

**Dependencies:**
Ensure you have the `TFT_eSPI` library installed via the Arduino Library Manager, and that your `User_Setup.h` in the library is correctly configured for an ILI9341 display.

---

## 2. Quick Start

Here is the absolute minimum code required to get the display running in your main project:

```cpp
#include <Arduino.h>
#include "ScannerDisplay.h"

// 1. Create the display instance
ScannerDisplay display;

// 2. Create the data container
VehicleData vehicle = { 0.0f, 100, ST_OK, "Starting...", 0 };

void setup() {
  // 3. Initialize the screen and draw the layout
  display.begin();
}

void loop() {
  // 4. Update your data container with real sensor readings
  vehicle.speed = 45.0f;     
  vehicle.direction = 1;     
  vehicle.battery = 85;      

  // 5. Send data to the display
  display.update(vehicle);
  
  delay(10); // small delay for stability
}
```

---

## 3. The `VehicleData` Contract

The `VehicleData` struct is the "bridge" between your robot and the screen. You fill out this struct, hand it to the display, and the display figures out exactly what needs to be redrawn.

```cpp
struct VehicleData {
  float    speed;            // Percentage (0.0 to 100.0)
  int      battery;          // Percentage (0 to 100)
  uint8_t  status;           // ST_OK, ST_WARN, or ST_ERROR
  char     msg[36];          // Short text message
  int8_t   direction;        // 1 = FWD, -1 = REV, 0 = STOP
};
```

### Breakdown of Fields:
*   **`speed`**: The current speed/throttle of the robot from `0.0f` to `100.0f`. Values above 75 turn orange (warning), and values above 90 turn red (critical).
*   **`battery`**: Integer from `0` to `100`. Drops below 30% turn orange; below 15% turn red.
*   **`status`**: Controls the color of the bottom message bar.
    *   `ST_OK` (0): Dark Green background. Everything is fine.
    *   `ST_WARN` (1): Red background with a `!` icon.
    *   `ST_ERROR` (2): Red background with an `X` icon.
*   **`msg`**: A string (up to 35 characters) displayed in the bottom bar. Use `strncpy` or `snprintf` to update this safely.
*   **`direction`**: Controls the FWD/REV/STOP badge under the speed text.
    *   `1`: Displays green **FWD**
    *   `-1`: Displays orange **REV**
    *   `0`: Displays grey **STOP**

---

## 4. Common Use Cases

### Case A: Normal Driving (Forward)
When the user pushes the joystick forward:

```cpp
vehicle.speed = 65.5f;   // 65.5% throttle
vehicle.direction = 1;   // Forward
vehicle.status = ST_OK;  // Green status bar
strncpy(vehicle.msg, "Cruising...", sizeof(vehicle.msg));

display.update(vehicle);
```

### Case B: Reversing
When the robot is moving backward:

```cpp
vehicle.speed = 20.0f;   // Slower speed for safety
vehicle.direction = -1;  // Reverse
vehicle.status = ST_WARN;// Turns bottom bar Red as a safety visual
strncpy(vehicle.msg, "Reversing!", sizeof(vehicle.msg));

display.update(vehicle);
```

### Case C: Stopped / Idling
When no input is detected:

```cpp
vehicle.speed = 0.0f;
vehicle.direction = 0;   // Stop
vehicle.status = ST_OK;
strncpy(vehicle.msg, "Ready. Waiting for input.", sizeof(vehicle.msg));

display.update(vehicle);
```

### Case D: Throwing a Hardware Error
If a sensor disconnects or a motor stalls:

```cpp
vehicle.speed = 0.0f;    // Force stop
vehicle.direction = 0;
vehicle.status = ST_ERROR; // Red bar with 'X'
strncpy(vehicle.msg, "ERR: Motor Stalled!", sizeof(vehicle.msg));

display.update(vehicle);
```

---

## 5. Performance & Refresh Rates

You **do not** need to manually throttle the `display.update()` function. 

You can call `display.update(vehicle)` thousands of times per second inside your main `loop()`. The `ScannerDisplay` class has a built-in throttle (`UI_REFRESH_MS`, defaulting to 150ms) and uses "delta-tracking". It will **only** redraw pixels on the screen if the data has actually changed since the last frame. This ensures your robot's main loop remains lightning fast and the screen never flickers.

---

## 6. Advanced Customization

If you need to change UI colors, speed thresholds, or layout positions later on, you no longer have to dig through multiple files. 
*   **Thresholds & Colors:** Open `ScannerDisplay.h`. All colors (RGB565 hex codes) and percentage thresholds are clearly defined at the top.
*   **Layout Geometry:** Open `ScannerDisplay.cpp`. All the `X`, `Y`, `Width`, and `Height` macros are defined at the very top of the file, hidden safely away from your main code.
