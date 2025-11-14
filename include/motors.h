#ifndef MOTORS_H
#define MOTORS_H

#include <Arduino.h>

// Current logical direction of the robot
typedef enum {
    MOTOR_DIR_STOPPED = 0,
    MOTOR_DIR_FORWARD,
    MOTOR_DIR_REVERSE,
    MOTOR_DIR_LEFT,
    MOTOR_DIR_RIGHT,
    MOTOR_DIR_BRAKE,
    MOTOR_DIR_COAST
} MotorDirection;

void motors_init();
void motors_set_speed(int speed);
int  motors_get_speed();            // NEW: read current PWM speed
MotorDirection motors_get_direction(); // NEW: read current direction

void motors_forward();
void motors_reverse();
void motors_left();
void motors_right();
void motors_brake();
void motors_coast();

// Optional helper (kept for compatibility)
void motors_update(bool fwd);

#endif
