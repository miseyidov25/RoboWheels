#ifndef MOTORS_H
#define MOTORS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Motor API
void motors_init(void);
void motors_set_speed(int speed);
void motors_forward(void);
void motors_reverse(void);
void motors_left(void);
void motors_correctleft(void);
void motors_right(void);
void motors_correctright(void);
void motors_coast(void);

typedef enum {
    MOTOR_STOP,
    MOTOR_FORWARD,
    MOTOR_REVERSE,
    MOTOR_LEFT,
    MOTOR_CORRECT_LEFT,
    MOTOR_RIGHT,
    MOTOR_CORRECT_RIGHT
} motor_direction_t;

// Globals
extern int motorSpeed;
extern int motorSpeedAuto;
extern int currentSpeedIndex;     

// Declare motorDirection as extern
extern volatile motor_direction_t motorDirection;

// Speed-level presets and index: defined in motors.cpp 
extern const int speedLevels[];
extern const int speedLevelsCount;

// Helper: declared here, implemented in main.cpp (only once)
int currentEffectiveSpeed(void);

#ifdef __cplusplus
}
#endif

#endif // MOTORS_H