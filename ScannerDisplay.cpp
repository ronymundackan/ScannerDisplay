#include "ScannerDisplay.h"

// --- Screen & Layout Constants (Hidden from the user) ---
#define SCREEN_W          320
#define SCREEN_H          240
#define SCREEN_ROTATION   2

#define HDR_H             0
#define STS_H             44
#define CONTENT_Y         2
#define CONTENT_H         (SCREEN_H - STS_H - 4)

// Speed panel (left)
#define SP_X              4
#define SP_Y              CONTENT_Y
#define SP_W              196
#define SP_H              CONTENT_H

// Battery panel (right)
#define BP_X              204
#define BP_Y              CONTENT_Y
#define BP_W              112
#define BP_H              CONTENT_H

// Speed Bar geometry
#define BAR_X             (SP_X + 8)
#define BAR_Y             (SP_Y + SP_H - 42)
#define BAR_W             (SP_W - 16)
#define BAR_H             16

// Battery Icon geometry
#define ICON_W            52
#define ICON_H            80
#define NUB_W             20
#define NUB_H             6
#define ICON_X            (BP_X + (BP_W - ICON_W) / 2)
#define ICON_Y            (BP_Y + 40)


ScannerDisplay::ScannerDisplay() : _tft(SCREEN_W, SCREEN_H) {
  _lastUiUpdate = 0;
  _prevSpeed    = -1.0f;
  _prevDir      = -99;
  _prevBatt     = -1;
  _prevStatus   = 255;
  _prevMsg[0]   = '\0';
}

void ScannerDisplay::begin() {
  _tft.init();
  _tft.setRotation(SCREEN_ROTATION);
  _tft.fillScreen(COL_BG);
  drawStaticFrames();
}

void ScannerDisplay::update(const VehicleData &data) {
  uint32_t now = millis();
  if (now - _lastUiUpdate >= UI_REFRESH_MS) {
    _lastUiUpdate = now;
    updateSpeedometer(data.speed, data.direction);
    updateBattery(data.battery);
    updateStatusBar(data.status, data.msg);
  }
}

void ScannerDisplay::drawStaticFrames() {
  // --- Speedometer Frame ---
  _tft.fillRoundRect(SP_X, SP_Y, SP_W, SP_H, 4, COL_PANEL);
  _tft.drawRoundRect(SP_X, SP_Y, SP_W, SP_H, 4, COL_DIM);

  _tft.setTextColor(COL_TEXT, COL_PANEL);
  _tft.setFreeFont(&FreeSansBold9pt7b);
  _tft.setCursor(SP_X + 8, SP_Y + 18);
  _tft.print("SPEED");

  _tft.drawRoundRect(BAR_X, BAR_Y, BAR_W, BAR_H, 3, COL_DIM);

  _tft.setFreeFont(&FreeSans9pt7b);
  _tft.setTextColor(COL_DIM, COL_PANEL);
  _tft.setCursor(BAR_X, BAR_Y + BAR_H + 14);
  _tft.print("0");
  char buf[8];
  snprintf(buf, sizeof(buf), "%d", (int)(SPEED_MAX / 2));
  _tft.setCursor(BAR_X + BAR_W / 2 - 12, BAR_Y + BAR_H + 14);
  _tft.print(buf);
  snprintf(buf, sizeof(buf), "%d", (int)SPEED_MAX);
  _tft.setCursor(BAR_X + BAR_W - 20, BAR_Y + BAR_H + 14);
  _tft.print(buf);
  _tft.setFreeFont(NULL);

  // --- Battery Frame ---
  _tft.fillRoundRect(BP_X, BP_Y, BP_W, BP_H, 4, COL_PANEL);
  _tft.drawRoundRect(BP_X, BP_Y, BP_W, BP_H, 4, COL_DIM);

  _tft.setTextColor(COL_TEXT, COL_PANEL);
  _tft.setFreeFont(&FreeSansBold9pt7b);
  _tft.setCursor(BP_X + 8, BP_Y + 18);
  _tft.print("BATTERY");
  _tft.setFreeFont(NULL);

  int nubX = ICON_X + (ICON_W - NUB_W) / 2;
  _tft.fillRoundRect(nubX, ICON_Y - NUB_H + 1, NUB_W, NUB_H, 2, COL_DIM);
  _tft.drawRoundRect(ICON_X, ICON_Y, ICON_W, ICON_H, 4, COL_DIM);

  // Reset trackers
  _prevSpeed  = -1.0f;
  _prevDir    = -99;
  _prevBatt   = -1;
  _prevStatus = 255;
}

uint16_t ScannerDisplay::getSpeedColor(float s) {
  if (s >= SPEED_MAX - 0.5f) return COL_DANGER;
  if (s >= SPEED_CRIT)       return COL_DANGER;
  if (s >= SPEED_WARN)       return COL_WARN;
  return COL_GOOD;
}

void ScannerDisplay::updateSpeedometer(float speed, int8_t dir) {
  speed = constrain(speed, 0.0f, SPEED_MAX);
  
  if (abs(speed - _prevSpeed) >= 0.05f) {
    _prevSpeed = speed;
    uint16_t col = getSpeedColor(speed);

    int vx = SP_X + 10, vy = SP_Y + 24;
    _tft.fillRect(vx, vy, 106, 44, COL_PANEL);
    _tft.setTextColor(col, COL_PANEL);
    _tft.setFreeFont(&FreeSansBold18pt7b);
    char buf[12];
    snprintf(buf, sizeof(buf), "%d%%", (int)speed);
    _tft.setCursor(vx, vy + 34);
    _tft.print(buf);

    int inner = BAR_W - 4;
    int fill  = (int)((speed / SPEED_MAX) * inner);
    fill = constrain(fill, 0, inner);
    _tft.fillRect(BAR_X + 2, BAR_Y + 2, inner, BAR_H - 4, COL_BAR_BG);
    if (fill > 0) {
      _tft.fillRect(BAR_X + 2, BAR_Y + 2, fill, BAR_H - 4, col);
    }
  }

  if (dir != _prevDir) {
    _prevDir = dir;
    int dx = SP_X + 10;
    int dy = SP_Y + 74;
    _tft.fillRect(dx, dy, 150, 40, COL_PANEL);
    _tft.setFreeFont(&FreeSansBold18pt7b);
    if (dir == 1) {
      _tft.setTextColor(COL_GOOD, COL_PANEL);
      _tft.setCursor(dx, dy + 32);
      _tft.print("FWD");
    } else if (dir == -1) {
      _tft.setTextColor(COL_WARN, COL_PANEL);
      _tft.setCursor(dx, dy + 32);
      _tft.print("REV");
    } else {
      _tft.setTextColor(COL_DIM, COL_PANEL);
      _tft.setCursor(dx, dy + 32);
      _tft.print("STOP");
    }
  }
  _tft.setFreeFont(NULL);
}

uint16_t ScannerDisplay::getBatteryColor(int pct) {
  if (pct <= BATT_CRIT) return COL_DANGER;
  if (pct <= BATT_WARN) return COL_WARN;
  return COL_GOOD;
}

void ScannerDisplay::updateBattery(int pct) {
  pct = constrain(pct, 0, 100);
  if (pct == _prevBatt) return;
  _prevBatt = pct;

  uint16_t col = getBatteryColor(pct);

  int pad   = 4;
  int innerW = ICON_W - pad * 2;
  int innerH = ICON_H - pad * 2;
  int fillH  = (int)((pct / 100.0f) * innerH);
  fillH = constrain(fillH, 0, innerH);

  _tft.fillRect(ICON_X + pad, ICON_Y + pad, innerW, innerH, COL_BAR_BG);
  if (fillH > 0) {
    _tft.fillRect(ICON_X + pad,
                  ICON_Y + pad + (innerH - fillH),
                  innerW, fillH, col);
  }

  int txtY = ICON_Y + ICON_H + 8;
  _tft.fillRect(BP_X + 8, txtY, BP_W - 16, 30, COL_PANEL);
  _tft.setTextColor(col, COL_PANEL);
  _tft.setFreeFont(&FreeSansBold12pt7b);
  char buf[8];
  snprintf(buf, sizeof(buf), "%d%%", pct);
  int tw = _tft.textWidth(buf);
  _tft.setCursor(BP_X + (BP_W - tw) / 2, txtY + 22);
  _tft.print(buf);

  _tft.setFreeFont(NULL);
}

uint16_t ScannerDisplay::getStatusBgColor(uint8_t st) {
  switch (st) {
    case ST_WARN:  return COL_DANGER;
    case ST_ERROR: return COL_DANGER;
    default:       return 0x0320;
  }
}

void ScannerDisplay::updateStatusBar(uint8_t st, const char *msg) {
  if (st == _prevStatus && strcmp(msg, _prevMsg) == 0) return;
  _prevStatus = st;
  strncpy(_prevMsg, msg, sizeof(_prevMsg) - 1);
  _prevMsg[sizeof(_prevMsg) - 1] = '\0';

  int barY = SCREEN_H - STS_H;
  uint16_t bg = getStatusBgColor(st);

  _tft.fillRoundRect(0, barY, SCREEN_W, STS_H, 3, bg);

  _tft.setTextColor(COL_TEXT, bg);
  _tft.setFreeFont(&FreeSansBold12pt7b);
  _tft.setCursor(10, barY + 30);
  if (st == ST_ERROR)      _tft.print("X ");
  else if (st == ST_WARN)  _tft.print("! ");
  else                     _tft.print("> ");

  _tft.print(msg);
  _tft.setFreeFont(NULL);
}
