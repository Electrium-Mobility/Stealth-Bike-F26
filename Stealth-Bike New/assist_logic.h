// assist_logic.h - pure calculations for pedal assist.
// No Arduino calls in here: everything takes plain numbers, so it can be
// compiled and unit-tested on a PC.
#pragma once

#include <stdint.h>

// Cadence in crank rpm from the most recent pulse intervals.
//   intervalsUs      : recent pulse-to-pulse intervals (order does not matter)
//   count            : how many entries of intervalsUs are valid (0 = none yet)
//   usSinceLastPulse : time from the last pulse until now
// Returns 0 if there are no intervals or the last pulse is older than timeoutUs.
// When pedaling slows, the time since the last pulse is used once it exceeds
// the average interval, so cadence falls smoothly instead of holding the old value.
float cadenceRpm(const uint32_t* intervalsUs, uint8_t count,
                 uint32_t usSinceLastPulse, uint8_t magnets, uint32_t timeoutUs);

// Piecewise-linear lookup. xs must be ascending. Clamps to the end values.
float interpolateCurve(const float* xs, const float* ys, uint8_t n, float x);

// Fraction of assist allowed at the current bike speed:
// 1 below (target - taper), 0 at or above target, linear in between.
float speedFactor(float speedKmh, float targetKmh, float taperKmh);

// Move `current` toward `target` by at most rate * dt.
float rampToward(float current, float target, float dtS,
                 float upAPerS, float downAPerS);

// Clamp to 0..maxA. NaN becomes 0.
float clampCurrent(float amps, float maxA);

// VESC electrical rpm -> bike speed in km/h (absolute value).
float erpmToKmh(float erpm, float polePairs, float wheelCircumferenceM);

// Target current before ramping, using the values from config.h:
// deadzone -> curve -> speed taper -> clamp. Returns 0 if speed is not valid.
float assistTargetCurrent(float cadenceRpm, float speedKmh, bool speedValid);

// Holds the ramped output between control steps. Starts at 0 A.
class AssistController {
 public:
  // One control step. dtS is the real elapsed time since the previous step.
  // Returns the current to send to the VESC (always 0..MAX_CURRENT_A).
  float update(float cadenceRpm, float speedKmh, bool speedValid, float dtS);

  float output() const { return output_; }
  float target() const { return target_; }

 private:
  float output_ = 0.0f;
  float target_ = 0.0f;
};
