// ============================================================
//  ScannerDisplay.h — Modular UI for Metal Scanner Robot
// ============================================================
#ifndef SCANNER_DISPLAY_H
#define SCANNER_DISPLAY_H

#include <TFT_eSPI.h>
#include <SPI.h>

// --- Speed Thresholds (%) ---
#define SPEED_MAX         100.0f
#define SPEED_WARN        75.0f
#define SPEED_CRIT        90.0f
#define SPEED_IDLE        2.0f

// --- Battery Thresholds (%) ---
#define BATT_WARN         30
#define BATT_CRIT         15

// --- Colour Palette (RGB565) ---
#define COL_BG            0x0000  // Black
#define COL_HEADER        0x10A2  // Dark slate
#define COL_PANEL         0x0841  // Panel fill
#define COL_TEXT          0xFFFF  // White
#define COL_DIM           0x8410  // Grey
#define COL_ACCENT        0x07FF  // Cyan
#define COL_GOOD          0x07E0  // Green
#define COL_WARN          0xFD20  // Orange
#define COL_DANGER        0xF800  // Red
#define COL_BAR_BG        0x2104  // Bar track

// --- Status Codes ---
#define ST_OK             0
#define ST_WARN           1
#define ST_ERROR          2

// --- Timing ---
#define UI_REFRESH_MS     150

// ============================================================
//  Vehicle Data Contract
// ============================================================
struct VehicleData {
  float    speed;            // %
  int      battery;          // 0-100 %
  uint8_t  status;           // ST_OK / ST_WARN / ST_ERROR
  char     msg[36];          // Status message string
  int8_t   direction;        // 1 = FWD, -1 = REV, 0 = STOP
};

// ============================================================
//  Display Class
// ============================================================
class ScannerDisplay {
public:
  ScannerDisplay();
  
  // Initializes the TFT and draws the static UI frames
  void begin();
  
  // Updates the display with new data (handles refresh rate automatically)
  void update(const VehicleData &data);

private:
  TFT_eSPI _tft;
  uint32_t _lastUiUpdate;

  // Internal state tracking for delta-redraws
  float   _prevSpeed;
  int8_t  _prevDir;
  int     _prevBatt;
  uint8_t _prevStatus;
  char    _prevMsg[36];

  // Helper drawing methods
  void drawStaticFrames();
  void updateSpeedometer(float speed, int8_t dir);
  void updateBattery(int pct);
  void updateStatusBar(uint8_t st, const char *msg);

  // Colour utilities
  uint16_t getSpeedColor(float s);
  uint16_t getBatteryColor(int pct);
  uint16_t getStatusBgColor(uint8_t st);
};

#endif // SCANNER_DISPLAY_H
