#include "motor.h"

// vesc uart functions 

// finds the current value based on the rpm, change in rpm, and current range
float inputMap(uint8_t rpm, uint8_t rpm_MAX, uint8_t rpm_START, float current_MAX, float current_MIN){
  float currentOutput;
  // sets current to 0 if calculated value is negative
  if (((current_MAX-current_MIN)/(rpm_MAX-rpm_START) * (rpm-rpm_START) + current_MIN) < 0){
    currentOutput = 0; 
  } else {
    currentOutput = (current_MAX-current_MIN)/(rpm_MAX-rpm_START) * (rpm-rpm_START) + current_MIN; 
  }

  return currentOutput;
}

// limits how fast the current can change and keeps it within range
float throttleFilter(float &prev, float curr){
  // ramp limit: cap the change from the previous value (in either direction)
  if ((curr-prev) > POSITIVE_THRESHOLD){
    curr = prev + POSITIVE_THRESHOLD;
  } else if ((prev-curr) > NEGATIVE_THRESHOLD){
    curr = prev - NEGATIVE_THRESHOLD;
  }
  // keep current in range (0 to CURRENT_LIMIT)
  if (curr < 0){
    curr = 0;
  } else if (curr > CURRENT_LIMIT){
    curr = CURRENT_LIMIT;
  }
  return curr;
}

// rpm functions

// finds if bike wheels are rotating
int checkForRotation(uint8_t *readingArray, uint8_t &status, uint8_t counter, uint8_t *finalReadingArray, uint8_t finalCounter){
  readingArray[counter] = digitalRead(D5);

    int sum = 0;
    for (int i{0}; i<INPUT_SAMPLES; i++){
      sum += readingArray[i];
    }

    if ((sum == 0 && status == 0) || (sum == INPUT_SAMPLES && status == 1)){ // If we are looking for all 0s and get 0s OR if we are looking for all 1s and get 1s
      status ^= 1;
      finalReadingArray[finalCounter] = 1;
      return 1;
    } else {
      finalReadingArray[finalCounter] = 0;
      return 0;
    }
}

// estimates how fast the wheels are turning: counts rotation transitions in the
// last ROTATION_SAMPLES samples. This is not true RPM; the time the window covers
// depends on how fast the main loop runs.
uint8_t rpmCalculation(uint8_t *finalReadingArray){
  uint8_t rpm = 0;
  for (int i{0}; i<ROTATION_SAMPLES; i++){
    rpm += finalReadingArray[i]; 
  }
  return rpm;
}
