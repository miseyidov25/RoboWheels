#ifndef PINS_H
#define PINS_H

// Motor pins (all PWM capable)
// H-bridge control pairs:
// IN1/IN2 = Left motor H-bridge inputs (control left wheel direction)
//   IN1: Arduino D9  (OC1A / PB1) - one direction
//   IN2: Arduino D6  (OC0A / PD6) - opposite direction
// IN3/IN4 = Right motor H-bridge inputs (control right wheel direction)
//   IN3: Arduino D5  (OC0B / PD5) - one direction
//   IN4: Arduino D10 (OC1B / PB2) - opposite direction
#define IN1 9
#define IN2 6
#define IN3 5
#define IN4 10

// Line sensor pins
#define LEFT_SENSOR 4
#define MIDDLE_SENSOR 3
#define RIGHT_SENSOR 2

// Echo sensor pins
#define TRIG_PIN 8
#define ECHO_PIN1 7  //middle forward
#define ECHO_PIN2 11 //left side
#define ECHO_PIN3 12 //right side
#define ECHO_PIN4 13 //back

// LED pins
#define LED_L A0
#define LED_R A1



#endif // PINS_H