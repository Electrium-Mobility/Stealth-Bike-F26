// assist_logic.cpp - pure calculations for pedal assist (no Arduino calls).
#include "assist_logic.h"

#include <math.h>

#include "config.h"

float cadenceRpm(const uint32_t* intervalsUs, uint8_t count,
                 uint32_t usSinceLastPulse, uint8_t magnets, uint32_t timeoutUs) {
  if (intervalsUs == nullptr || count == 0 || magnets == 0) return 0.0f;
  if (usSinceLastPulse > timeoutUs) return 0.0f;

  uint64_t sumUs = 0;
  for (uint8_t i = 0; i < count; i++) sumUs += intervalsUs[i];
  float intervalUs = (float)sumUs / (float)count;

  // Rider slowing down: the pulse we are waiting for is already later than the
  // average interval, so the real cadence is at most this.
  if ((float)usSinceLastPulse > intervalUs) intervalUs = (float)usSinceLastPulse;
  if (intervalUs <= 0.0f) return 0.0f;

  return 60.0e6f / (intervalUs * (float)magnets);
}

float interpolateCurve(const float* xs, const float* ys, uint8_t n, float x) {
  if (xs == nullptr || ys == nullptr || n == 0) return 0.0f;
  if (x <= xs[0]) return ys[0];
  if (x >= xs[n - 1]) return ys[n - 1];
  for (uint8_t i = 1; i < n; i++) {
    if (x <= xs[i]) {
      const float span = xs[i] - xs[i - 1];
      if (span <= 0.0f) return ys[i];
      const float t = (x - xs[i - 1]) / span;
      return ys[i - 1] + t * (ys[i] - ys[i - 1]);
    }
  }
  return ys[n - 1];
}

float speedFactor(float speedKmh, float targetKmh, float taperKmh) {
  if (speedKmh >= targetKmh) return 0.0f;
  if (taperKmh <= 0.0f) return 1.0f;
  const float f = (targetKmh - speedKmh) / taperKmh;
  return f > 1.0f ? 1.0f : f;
}

float rampToward(float current, float target, float dtS,
                 float upAPerS, float downAPerS) {
  if (dtS <= 0.0f) return current;
  if (target > current) {
    const float next = current + upAPerS * dtS;
    return next > target ? target : next;
  }
  if (target < current) {
    const float next = current - downAPerS * dtS;
    return next < target ? target : next;
  }
  return current;
}

float clampCurrent(float amps, float maxA) {
  if (!(amps > 0.0f)) return 0.0f;  // also catches NaN
  return amps > maxA ? maxA : amps;
}

float erpmToKmh(float erpm, float polePairs, float wheelCircumferenceM) {
  if (polePairs <= 0.0f) return 0.0f;
  const float wheelRpm = fabsf(erpm) / polePairs;
  return wheelRpm * wheelCircumferenceM * 60.0f / 1000.0f;  // m/min -> km/h
}

float assistTargetCurrent(float cadence, float speedKmh, bool speedValid) {
  if (!speedValid) return 0.0f;                     // unknown speed: fail safe
  if (!(cadence >= CADENCE_DEADZONE_RPM)) return 0.0f;  // deadzone (also NaN)

  const float base = interpolateCurve(CURVE_CADENCE_RPM, CURVE_CURRENT_A,
                                      CURVE_POINTS, cadence);
  const float scaled = base * speedFactor(speedKmh, TARGET_SPEED_KMH, SPEED_TAPER_KMH);
  return clampCurrent(scaled, MAX_CURRENT_A);
}

float AssistController::update(float cadence, float speedKmh, bool speedValid, float dtS) {
  target_ = assistTargetCurrent(cadence, speedKmh, speedValid);

  if (!(dtS > 0.0f)) dtS = 0.0f;
  if (dtS > MAX_RAMP_DT_S) dtS = MAX_RAMP_DT_S;

  output_ = clampCurrent(
      rampToward(output_, target_, dtS, RAMP_UP_A_PER_S, RAMP_DOWN_A_PER_S),
      MAX_CURRENT_A);
  return output_;
}
