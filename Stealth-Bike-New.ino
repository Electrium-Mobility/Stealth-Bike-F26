// Stealth-Bike-New.ino - cadence-based pedal assist for a VESC-driven e-bike.
//
// Hardware glue only: interrupt, timing, VESC UART. All math is in
// assist_logic.cpp and all tunable values are in config.h.

#include <VescUart.h>
#include <SoftwareSerial.h>

#include "config.h"
#include "assist_logic.h"

#ifndef IRAM_ATTR
#define IRAM_ATTR  // ESP8266/ESP32 need ISRs in IRAM; other boards don't
#endif

VescUart vesc;
SoftwareSerial vescSerial(VESC_RX_PIN, VESC_TX_PIN);
AssistController assist;

// ---- Pedal sensor state, written by the ISR ----
volatile uint32_t pasIntervalsUs[PAS_AVG_PULSES];
volatile uint8_t  pasHead      = 0;      // next slot to write
volatile uint8_t  pasCount     = 0;      // valid intervals in the buffer
volatile uint32_t pasLastUs    = 0;      // micros() of the last accepted pulse
volatile bool     pasHavePulse = false;

// ---- Loop timing / telemetry ----
uint32_t lastControlMs     = 0;
uint32_t lastTelemetryMs   = 0;
uint32_t lastTelemetryOkMs = 0;
bool     haveTelemetry     = false;
float    speedKmh          = 0.0f;
uint8_t  debugStep         = 0;

void IRAM_ATTR onPasPulse() {
  const uint32_t now = micros();
  if (pasHavePulse) {
    const uint32_t dt = now - pasLastUs;  // unsigned math handles micros() wrap
    if (dt < PAS_DEBOUNCE_US) return;     // bounce / noise
    if (dt > PAS_TIMEOUT_US) {
      pasCount = 0;                       // rider had stopped: start a fresh average
      pasHead = 0;
    } else {
      pasIntervalsUs[pasHead] = dt;
      pasHead = (uint8_t)((pasHead + 1) % PAS_AVG_PULSES);
      if (pasCount < PAS_AVG_PULSES) pasCount++;
    }
  }
  pasLastUs = now;
  pasHavePulse = true;
}

// Copy the ISR state atomically and turn it into a cadence.
static float readCadenceRpm() {
  uint32_t intervals[PAS_AVG_PULSES];
  noInterrupts();
  const uint32_t sinceLast = micros() - pasLastUs;
  if (pasHavePulse && sinceLast > PAS_TIMEOUT_US) {
    // Forget the old pulses. Without this, micros() wrapping (~71 min) could
    // make a pulse from long ago look recent again.
    pasHavePulse = false;
    pasCount = 0;
    pasHead = 0;
  }
  const bool    have  = pasHavePulse;
  const uint8_t count = pasCount;
  for (uint8_t i = 0; i < count; i++) intervals[i] = pasIntervalsUs[i];
  interrupts();

  if (!have) return 0.0f;
  return cadenceRpm(intervals, count, sinceLast, PAS_MAGNETS, PAS_TIMEOUT_US);
}

static void pollTelemetry(uint32_t nowMs) {
  if (nowMs - lastTelemetryMs < TELEMETRY_PERIOD_MS) return;
  lastTelemetryMs = nowMs;
  if (vesc.getVescValues()) {  // blocks until the VESC replies or times out
    speedKmh = erpmToKmh(vesc.data.rpm, MOTOR_POLE_PAIRS, WHEEL_CIRCUMFERENCE_M);
    lastTelemetryOkMs = millis();
    haveTelemetry = true;
  }
}

void setup() {
  Serial.begin(SERIAL_BAUD);
  vescSerial.begin(VESC_BAUD);
  vesc.setSerialPort(&vescSerial);

  vesc.setCurrent(0.0f);  // start at 0 A

  pinMode(PAS_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(PAS_PIN), onPasPulse, FALLING);

  lastControlMs = millis();
  lastTelemetryMs = lastControlMs;
}

void loop() {
  const uint32_t nowMs = millis();
  if (nowMs - lastControlMs < CONTROL_PERIOD_MS) return;

  const float dtS = (float)(nowMs - lastControlMs) / 1000.0f;  // real elapsed time
  lastControlMs = nowMs;

  const float cadence = readCadenceRpm();
  const bool speedValid = haveTelemetry && (nowMs - lastTelemetryOkMs <= TELEMETRY_STALE_MS);
  const float amps = assist.update(cadence, speedKmh, speedValid, dtS);
  vesc.setCurrent(amps);

  // Telemetry right after the control step, so its blocking read eats into
  // idle time instead of delaying the next setCurrent().
  pollTelemetry(millis());

  if (DEBUG_PRINT && ++debugStep >= DEBUG_EVERY_STEPS) {
    debugStep = 0;
    Serial.print("cad=");     Serial.print(cadence, 1);
    Serial.print(" kmh=");    Serial.print(speedKmh, 1);
    Serial.print(speedValid ? "" : "(stale)");
    Serial.print(" target="); Serial.print(assist.target(), 2);
    Serial.print(" out=");    Serial.println(amps, 2);
  }
}
