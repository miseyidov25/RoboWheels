#include "motors.h"
#include <Arduino.h>

// Motor pins (all PWM capable)
const int IN4 = 10;
const int IN1 = 9;
const int IN2 = 6;
const int IN3 = 5;

// Motor speed (0–255)
int motorSpeed = 185;

// Speed levels for Bluetooth control
int speedLevels[] = {120, 150, 185, 220};
int currentSpeedIndex = 2;  // default = f3 (185)

void motors_init() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);

    motors_coast();
}

void motors_set_speed(int speed) {
    motorSpeed = constrain(speed, 0, 255);
}

void motors_forward() {
    // Left motor: IN1 forward, IN2 reverse
    analogWrite(IN1, motorSpeed);
    analogWrite(IN2, 0);

    // Right motor: IN3 forward, IN4 reverse
    analogWrite(IN3, motorSpeed);
    analogWrite(IN4, 0);
}

void motors_reverse() {
    analogWrite(IN1, 0);
    analogWrite(IN2, motorSpeed);
    analogWrite(IN3, 0);
    analogWrite(IN4, motorSpeed);
}

void motors_left() {
    // Left motor backward, right motor forward
    analogWrite(IN1, 0);
    analogWrite(IN2, motorSpeed);
    analogWrite(IN3, motorSpeed);
    analogWrite(IN4, 0);
}

void motors_right() {
    // Left motor forward, right motor backward
    analogWrite(IN1, motorSpeed);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, motorSpeed);
}

void motors_brake() {
    // Both pins high - electrical brake
    analogWrite(IN1, 255);
    analogWrite(IN2, 255);
    analogWrite(IN3, 255);
    analogWrite(IN4, 255);
}

void motors_coast() {
    // All pins low - free spin
    analogWrite(IN1, 0);
    analogWrite(IN2, 0);
    analogWrite(IN3, 0);
    analogWrite(IN4, 0);
}

void motors_update(bool fwd) {
    if (fwd) motors_forward();
    else motors_coast();
}
