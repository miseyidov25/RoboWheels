#include "motors.h"
#include "pins.h"
#include <Arduino.h>
#include "bt.h"    // for currentEffectiveSpeed()


extern int currentEffectiveSpeed();  // declare function
int motorDirection = 0;   // 0=stop, 1=fwd, -1=rev, 2=left, 3=right


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
  motorDirection = 1;
}
void motors_reverse() {
  int s = currentEffectiveSpeed();
  analogWrite(IN1, 0);
  analogWrite(IN2, s);
  analogWrite(IN3, 0);
  analogWrite(IN4, s);
  motorDirection = -1;
}
void motors_left() {
  int s = currentEffectiveSpeed();
  // Turn left by reversing left motor and driving right motor forward
  analogWrite(IN1, 0);      // left motor forward pin off
  analogWrite(IN2, s);      // left motor reverse pin on
  analogWrite(IN3, s);      // right motor forward pin on
  analogWrite(IN4, 0);      // right motor reverse pin off
  motorDirection = 2;
}
void motors_correctleft() {
  // Slight left correction: reduce left forward contribution (soft turn)
  int s = currentEffectiveSpeed();
  int leftSpeed = s - 30;
  if (leftSpeed < 0) leftSpeed = 0;
  // Keep left motor forward reduced, right motor forward normal
  analogWrite(IN1, leftSpeed);
  analogWrite(IN2, 0);
  analogWrite(IN3, s);
  analogWrite(IN4, 0);
  motorDirection = 2;
}
void motors_right() {
  int s = currentEffectiveSpeed();
  // Turn right by driving left motor forward and reversing right motor
  analogWrite(IN1, s);    // left motor forward
  analogWrite(IN2, 0);    // left motor reverse off
  analogWrite(IN3, 0);    // right motor forward off
  analogWrite(IN4, s);    // right motor reverse
  motorDirection = 3;
}
void motors_correctright() {
  // Slight right correction: reduce right forward contribution (soft turn)
  int s = currentEffectiveSpeed();
  int rightSpeed = s - 30;
  if (rightSpeed < 0) rightSpeed = 0;
  analogWrite(IN1, s);
  analogWrite(IN2, 0);
  analogWrite(IN3, rightSpeed);
  analogWrite(IN4, 0);
  motorDirection = 3;
}
void motors_brake() {
  analogWrite(IN1, 255);
  analogWrite(IN2, 255);
  analogWrite(IN3, 255);
  analogWrite(IN4, 255);
  motorDirection = 0;
}
void motors_coast() {
  analogWrite(IN1, 0);
  analogWrite(IN2, 0);
  analogWrite(IN3, 0);
  analogWrite(IN4, 0);
  motorDirection = 0;
}
