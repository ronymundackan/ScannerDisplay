// ============================================================
//  dummy_data.h — Simulated Vehicle Data for Bench Testing
//  Cycles through realistic scenarios so you can verify the
//  display without connecting the real vehicle.
//
//  Scenarios (loops automatically):
//    1. Accelerate  0 → SPEED_MAX
//    2. Cruise at max for a moment
//    3. Decelerate back to 0
//    4. "Vehicle not moving" warning
//    5. "Unknown error occurred" error
//    6. Recovery back to normal
// ============================================================
#ifndef DUMMY_DATA_H
#define DUMMY_DATA_H

#include "ScannerDisplay.h"

namespace Dummy {

  // Scenario phases
  enum Phase : uint8_t {
    ACCEL,      // speed ramps up
    CRUISE,     // hold max
    DECEL,      // speed ramps down
    IDLE_WARN,  // stopped warning
    ERROR_SIM,  // error state
    RECOVER,    // back to normal
    PHASE_COUNT
  };

  static Phase    _phase    = ACCEL;
  static float    _speed    = 0.0f;
  static int      _batt     = 95;
  static uint32_t _lastStep = 0;
  static uint16_t _ticks    = 0;      // ticks inside current phase

  // How many ticks each phase lasts (at DUMMY_STEP_MS intervals)
  static const uint16_t PHASE_LEN[] = {
    40,   // ACCEL   ~4.8 s
    15,   // CRUISE  ~1.8 s
    40,   // DECEL   ~4.8 s
    20,   // IDLE    ~2.4 s
    20,   // ERROR   ~2.4 s
    10    // RECOVER ~1.2 s
  };

  static void nextPhase() {
    _phase = (Phase)(((int)_phase + 1) % PHASE_COUNT);
    _ticks = 0;
  }

  // Call every loop iteration; fills `data` with simulated values
  void step(VehicleData &data, uint32_t now) {
    if (now - _lastStep < DUMMY_STEP_MS) {
      // Just return current state
      data.speed   = _speed;
      data.battery = _batt;
      return;
    }
    _lastStep = now;
    _ticks++;

    // Drain battery slowly
    if (_ticks % 30 == 0 && _batt > 5) _batt--;

    switch (_phase) {
      case ACCEL:
        _speed += SPEED_MAX / (float)PHASE_LEN[ACCEL];
        if (_speed > SPEED_MAX) _speed = SPEED_MAX;
        data.status = ST_OK;
        data.direction = 1;
        snprintf(data.msg, sizeof(data.msg), "Scanning...");
        break;

      case CRUISE:
        _speed = SPEED_MAX;
        data.status = ST_OK;
        data.direction = 1;
        snprintf(data.msg, sizeof(data.msg), "Cruise  %.0f%%", _speed);
        break;

      case DECEL:
        _speed -= SPEED_MAX / (float)PHASE_LEN[DECEL];
        if (_speed < 0) _speed = 0;
        data.status = ST_OK;
        data.direction = 1;
        snprintf(data.msg, sizeof(data.msg), "Decelerating...");
        break;

      case IDLE_WARN:
        _speed = 0;
        data.status = ST_WARN;
        data.direction = 0;
        snprintf(data.msg, sizeof(data.msg), "Vehicle not moving");
        break;

      case ERROR_SIM:
        _speed = 0;
        data.status = ST_ERROR;
        data.direction = 0;
        snprintf(data.msg, sizeof(data.msg), "Unknown error occurred");
        break;

      case RECOVER:
        _speed = 0;
        _batt  = 95;            // "battery swapped"
        data.status = ST_OK;
        data.direction = -1;    // show reverse for testing
        snprintf(data.msg, sizeof(data.msg), "Reversing...");
        break;
    }

    data.speed   = _speed;
    data.battery = _batt;

    if (_ticks >= PHASE_LEN[_phase]) nextPhase();
  }

} // namespace Dummy

#endif // DUMMY_DATA_H
