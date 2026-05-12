// ============================================================
//  Metal Scanner — Main Robot Controller
// ============================================================

#include "ScannerDisplay.h"

// Set to 1 to simulate movement/battery drain without real hardware
#define DUMMY_MODE 1

#if DUMMY_MODE
  #define DUMMY_STEP_MS 120
  #include "dummy_data.h"
#endif

// 1. Create the display object and your data struct
ScannerDisplay display;
VehicleData vehicle = { 0.0f, 100, ST_OK, "Initialising...", 0 };

// ============================================================
//  SETUP
// ============================================================
void setup() {
  Serial.begin(115200);
  Serial.println("[SCANNER] Booting...");

  // Initialize the TFT screen and draw the static UI frames
  display.begin();

  Serial.println("[SCANNER] Ready.");
}

// ============================================================
//  LOOP
// ============================================================
void loop() {
  uint32_t now = millis();

  // ---------------------------------------------------
  // PART A: GET YOUR ROBOT DATA 
  // ---------------------------------------------------
  #if DUMMY_MODE
    Dummy::step(vehicle, now);
  #else
    // *** REAL HARDWARE ***
    // Example:
    // vehicle.speed     = readSpeedSensorPercent();
    // vehicle.battery   = readBatteryADC();
    // vehicle.direction = readDirectionSwitch();
    // vehicle.status    = checkErrors();
    // strncpy(vehicle.msg, "Cruising...", sizeof(vehicle.msg));
  #endif

  // ---------------------------------------------------
  // PART B: SEND IT TO THE DISPLAY
  // ---------------------------------------------------
  display.update(vehicle);
}