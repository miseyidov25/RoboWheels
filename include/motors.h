#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

void motors_init();
void motors_forward();
void motors_reverse();
void motors_left();
void motors_right();
void motors_brake();
void motors_coast();

// High-level update function for main loop
void motors_update(bool fwd, bool rev, bool left, bool right, bool obstacle);

#endif
