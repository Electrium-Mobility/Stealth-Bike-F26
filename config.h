// config.h - every tunable value for the pedal-assist firmware lives here.
// This header has no Arduino dependencies so the logic can be compiled and
// unit-tested on a PC.
#pragma once

#include <stdint.h>

// ---------------------------------------------------------------------------
// Hardware (pin macros are only expanded inside the .ino, so D5 does not need
// to exist when compiling the logic on a PC)
// ---------------------------------------------------------------------------
#define PAS_PIN        D5   // pedal-assist hall sensor signal
#define VESC_RX_PIN    13   // ESP8266 GPIO13 receives from VESC TX (same as existing sketch)
#define VESC_TX_PIN    15   // ESP8266 GPIO15 transmits to VESC RX (same as existing sketch)

constexpr uint32_t VESC_BAUD    = 9600;    // must match the VESC UART app setting
constexpr uint32_t SERIAL_BAUD  = 115200;  // USB debug output

// ---------------------------------------------------------------------------
// Control loop
// ---------------------------------------------------------------------------
constexpr uint32_t CONTROL_PERIOD_MS = 40;   // 25 Hz
// If a step is late (e.g. a slow UART read), the ramp uses at most this much
// elapsed time, so a stall can never produce a large jump in current.
constexpr float    MAX_RAMP_DT_S     = 0.1f;

// ---------------------------------------------------------------------------
// Pedal (cadence) sensor
// ---------------------------------------------------------------------------
constexpr uint8_t  PAS_MAGNETS      = 12;     // pulses per crank revolution
constexpr uint32_t PAS_DEBOUNCE_US  = 5000;   // ignore edges closer than this (150 rpm = 33 ms/pulse)
constexpr uint8_t  PAS_AVG_PULSES   = 6;      // intervals averaged for cadence (6 = half a crank turn)
// No pulse for this long means "not pedaling". 400 ms between pulses equals
// 12.5 rpm with 12 magnets. Keep it longer than one pulse interval at
// CADENCE_DEADZONE_RPM (333 ms at 15 rpm), otherwise the timeout becomes the
// effective deadzone.
constexpr uint32_t PAS_TIMEOUT_MS   = 400;
constexpr uint32_t PAS_TIMEOUT_US   = PAS_TIMEOUT_MS * 1000UL;

// ---------------------------------------------------------------------------
// Assist behavior
// ---------------------------------------------------------------------------
constexpr float CADENCE_DEADZONE_RPM = 15.0f;  // below this cadence: 0 A
constexpr float TARGET_SPEED_KMH     = 25.0f;  // assist fades out as the bike reaches this speed
constexpr float SPEED_TAPER_KMH      = 3.0f;   // full assist below (target - taper), 0 A at target
constexpr float MAX_CURRENT_A        = 15.0f;  // hard output limit (output is always 0..MAX_CURRENT_A)

// Ramp rates, applied every control step. 0 -> 15 A takes ~1.9 s up, ~1.5 s down.
constexpr float RAMP_UP_A_PER_S      = 8.0f;
constexpr float RAMP_DOWN_A_PER_S    = 10.0f;

// Cadence -> current curve (piecewise linear, cadence must be ascending).
// Slow pedaling gets more current, fast pedaling gets less.
// Below the first point the first value is used, above the last point the
// last value is used. Values above MAX_CURRENT_A are clamped.
constexpr float CURVE_CADENCE_RPM[] = { 15.0f, 30.0f, 50.0f, 70.0f, 90.0f };
constexpr float CURVE_CURRENT_A[]   = { 15.0f, 13.0f,  9.0f,  5.0f,  2.0f };
constexpr uint8_t CURVE_POINTS = sizeof(CURVE_CADENCE_RPM) / sizeof(CURVE_CADENCE_RPM[0]);
static_assert(sizeof(CURVE_CADENCE_RPM) == sizeof(CURVE_CURRENT_A),
              "cadence curve arrays must have the same length");
static_assert(CURVE_POINTS >= 1, "cadence curve needs at least one point");

// ---------------------------------------------------------------------------
// Bike speed from VESC telemetry (needed to know when the target speed is reached)
// ---------------------------------------------------------------------------
constexpr uint32_t TELEMETRY_PERIOD_MS = 200;   // how often to poll getVescValues()
constexpr uint32_t TELEMETRY_STALE_MS  = 1000;  // no valid reading for this long -> 0 A
constexpr float    MOTOR_POLE_PAIRS    = 23.0f; // hub motor; ERPM / pole pairs = wheel rpm
constexpr float    WHEEL_CIRCUMFERENCE_M = 2.0f;

// ---------------------------------------------------------------------------
// Debug
// ---------------------------------------------------------------------------
constexpr bool    DEBUG_PRINT        = true;
constexpr uint8_t DEBUG_EVERY_STEPS  = 5;   // print every 5th control step (5 Hz)
