#include "motors.h"
#include "pins.h"
#include <Arduino.h>
#include "bt.h"    // for currentEffectiveSpeed()


extern int currentEffectiveSpeed();  // declare function


// Definitions (exactly one definition here for speedLevels and index)
int speedLevels[] = {120, 150, 185, 220};
int currentSpeedIndex = 2;                     // default -> speedLevels[2] = 185
const int speedLevelsCount = sizeof(speedLevels) / sizeof(speedLevels[0]);

void motors_init() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    motors_coast();
}

void motors_set_speed(int speed) {
    // motorSpeed is extern (defined in main.cpp)
    motorSpeed = constrain(speed, 0, 255);
}

// === Motor Control Functions (PWM) ===
void motors_forward() {
  int s = currentEffectiveSpeed();
  analogWrite(IN1, s);
  analogWrite(IN2, 0);
  analogWrite(IN3, s);
  analogWrite(IN4, 0);
}
void motors_reverse() {
  int s = currentEffectiveSpeed();
  analogWrite(IN1, 0);
  analogWrite(IN2, s);
  analogWrite(IN3, 0);
  analogWrite(IN4, s);
}
void motors_left() {
  int s = currentEffectiveSpeed();
  analogWrite(IN1, s);
  analogWrite(IN2, s);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}
void motors_correctleft() {
  // Slight left turn - reduce left side speed
  int s = currentEffectiveSpeed();
  analogWrite(IN1, s - 30);
  analogWrite(IN2, 0);
  analogWrite(IN3, s);
  analogWrite(IN4, 0);
}
void motors_right() {
  int s = currentEffectiveSpeed();
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, s);
  analogWrite(IN4, s);
}
void motors_correctright() {
  // Slight right turn - reduce right side speed
  int s = currentEffectiveSpeed();
  analogWrite(IN1, s);
  analogWrite(IN2, 0);
  analogWrite(IN3, s - 30);
  analogWrite(IN4, 0);
}
void motors_brake() {
  analogWrite(IN1, 255);
  analogWrite(IN2, 255);
  analogWrite(IN3, 255);
  analogWrite(IN4, 255);
}
void motors_coast() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
}
