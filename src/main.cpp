#include <Arduino.h>
#include "pins.h"
#include "motors.h"
#include "line.h"
#include "bt.h"

// === Mode Control ===
enum Mode { MANUAL, AUTONOMOUS };
Mode currentMode = MANUAL;

// === Speed Control ===
// Define the motorSpeed variables exactly once, here:
int motorSpeed = 230;        // Adjust speed here for MANUAL (0–255)
int motorSpeedAuto = 110;    // Adjust speed here for AUTONOMOUS (0–255)

// === Timing for LED Blink ===
unsigned long previousMillis = 0;

// helper to return current effective speed depending on mode
int currentEffectiveSpeed() {
  return (currentMode == AUTONOMOUS) ? motorSpeedAuto : motorSpeed;
}

// === Setup ===
void setup() {
  Serial.begin(9600);  // HC-05 default baud rate

  // initialize motors module (pins + coast)
  motors_init();

  // Line sensor pins
  pinMode(LEFT_SENSOR, INPUT);
  pinMode(MIDDLE_SENSOR, INPUT);
  pinMode(RIGHT_SENSOR, INPUT);
  pinMode(LINE_LED, OUTPUT);

  bt_init();
}

// === Main Loop ===
void loop() {
  // handle Bluetooth / serial commands
  bt_update();

  // Autonomous line-following mode
  if (currentMode == AUTONOMOUS) {
    line_update();
    return; // skip manual commands while autonomous
  }

  // If not autonomous, nothing else required — bt_update handles manual movement
}
