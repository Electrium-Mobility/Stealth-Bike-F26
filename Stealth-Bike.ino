#include "oled.h"
#include "motor.h"

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R1, /* reset=*/U8X8_PIN_NONE, /* clock=*/SCL, /* data=*/SDA);
/** Initiate SoftwareSerial class */

// libraries
#include <VescUart.h>
#include <SoftwareSerial.h>

/** Initiate VescUart class */
VescUart vesc;

/** Initiate SoftwareSerial class */
SoftwareSerial vescSerial(13, 15);

uint8_t finalReadingArray[ROTATION_SAMPLES]; // keeps track of how often wheels are rotating
uint8_t inputArray[INPUT_SAMPLES] = { 1, 1, 1, 1 }; // finds if wheels are rotating or not
uint8_t counter = 0; // loops through all INPUT_SAMPLES input array values
uint8_t status = 0; // if wheels are rotating or not (rotating == 1, not rotating == 0)
uint8_t finalCounter = 0; // loops through all ROTATION_SAMPLES final reading array values
float currentOutput = 0; // current value
float prevOutput = 0; // previous current value
uint8_t percent = 100; // battery percentage
uint8_t speed = 100; // speed of bike


void setup() {
  u8g2.begin();

  /** Setup Serial port to display data */
  Serial.begin(9600);

  /** Setup SoftwareSerial port */
  vescSerial.begin(9600);

  /** Define which ports to use as UART */
  vesc.setSerialPort(&vescSerial);
  for (int i = 0; i < ROTATION_SAMPLES; i++) {
    finalReadingArray[i] = 0;
  }
  pinMode(D5, INPUT_PULLUP);
}


void loop() {
  prevOutput = currentOutput; // updates previous current value
  /** Call the function getVescValues() to acquire data from VESC */
  if ( vesc.getVescValues() ) {
    float rawSpeed = (vesc.data.rpm) * WHEEL_CIRCUMFERENCE * 60/23000; // calculate speed based on rpm
    float rawPercent = ((vesc.data.inpVoltage) - 36 )/ (MAX_VOLTAGE - 36) * 100; // calculate battery percent based on voltage
    // clamp before converting so out-of-range values don't wrap around
    speed = (uint8_t)constrain(rawSpeed, 0, 255);
    percent = (uint8_t)constrain(rawPercent, 0, 100);
  }

    // displays values on oled display (draw everything, then send one frame)
    u8g2.clearBuffer();
    showBatteryLevel(percent, speed, u8g2);
    ShowBatteryPercent(percent, u8g2);
    iShowSpeed(speed, u8g2);
    u8g2.sendBuffer();

  for (int i{0}; i < ROTATION_SAMPLES; i++) {
  checkForRotation(inputArray, status, counter, finalReadingArray, finalCounter); // checks if bike is rotating
  currentOutput = rpmCalculation(finalReadingArray); // finds how fast the wheels are turning (rpm)
  currentOutput = inputMap(currentOutput, RPM_MAX, RPM_MIN , CURRENT_MAX, CURRENT_MIN); // finds current
  currentOutput = throttleFilter(prevOutput, currentOutput); // adjusts current value, ensuring it is within range
  Serial.println(currentOutput);
  vesc.setCurrent(currentOutput); // sets the motor current

  counter = (counter + 1) % INPUT_SAMPLES; // checks input value INPUT_SAMPLES times, to reduce noise and ensure wheels are rotating or not
  finalCounter = (finalCounter + 1) % ROTATION_SAMPLES; // sets all values in reading array, to keep track of previous rotation values to find rpm
  }


  delay(1);
}