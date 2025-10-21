#ifndef BT_H
#define BT_H

// Motor pin definitions
#define IN1 7
#define IN2 6
#define IN3 4
#define IN4 2
#define ENA 3  // PWM pin for left motor
#define ENB 5  // PWM pin for right motor

// Motor control functions
void motors_forward(void);
void motors_reverse(void);
void motors_left(void);
void motors_right(void);
void motors_brake(void);
void motors_coast(void);

#endif
