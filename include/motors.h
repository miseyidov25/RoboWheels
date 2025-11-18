#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

// Motor API
void motors_init();
void motors_set_speed(int speed);
void motors_forward();
void motors_reverse();
void motors_left();
void motors_right();
void motors_brake();
void motors_coast();

// Globals: defined exactly once in main.cpp or motors.cpp
extern int motorSpeed;          // defined in main.cpp
extern int motorSpeedAuto;      // defined in main.cpp

// Speed-level presets and index: defined in motors.cpp (storage)
extern int speedLevels[];
extern int currentSpeedIndex;
extern const int speedLevelsCount;

// Helper: declared here, implemented in main.cpp (only once)
int currentEffectiveSpeed();

#endif // MOTORS_H
