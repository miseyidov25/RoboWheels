#include "motors.h"
#include "pins.h"
#include <avr/io.h>
#include "bt.h"    // for currentEffectiveSpeed()


extern int currentEffectiveSpeed();  // declare function
int motorDirection = 0;   // 0=stop, 1=fwd, -1=rev, 2=left, 3=right


// Definitions (exactly one definition here for speedLevels and index)
int speedLevels[] = {120, 150, 185, 220};
int currentSpeedIndex = 2;                     // default -> speedLevels[2] = 185
const int speedLevelsCount = sizeof(speedLevels) / sizeof(speedLevels[0]);

void motors_init() {
    // Set motor pins as outputs
    DDRB |= (1 << PB1) | (1 << PB2);  // IN1 (9), IN4 (10)
    DDRD |= (1 << PD5) | (1 << PD6);  // IN3 (5), IN2 (6)
    
    // Setup Timer0 for fast PWM, prescaler 64
    TCCR0A = (1 << COM0A1) | (1 << COM0B1) | (1 << WGM01) | (1 << WGM00);
    TCCR0B = (1 << CS01) | (1 << CS00);
    
    // Setup Timer1 for fast PWM 8-bit, prescaler 64
    TCCR1A = (1 << COM1A1) | (1 << COM1B1) | (1 << WGM11);
    TCCR1B = (1 << WGM13) | (1 << WGM12) | (1 << CS11) | (1 << CS10);
    
    motors_coast();
}

void motors_set_speed(int speed) {
    // motorSpeed is extern (defined in main.cpp)
    if (speed < 0) speed = 0;
    if (speed > 255) speed = 255;
    motorSpeed = speed;
}

// Motor Control Functions (PWM) 
void motors_forward() {
  int s = currentEffectiveSpeed();
  // left motor forward
  OCR1A = s;  // IN1
  OCR0A = 0;  // IN2
  // right motor forward
  OCR0B = s;  // IN3
  OCR1B = 0;  // IN4
  motorDirection = 1;
}
void motors_reverse() {
  int s = currentEffectiveSpeed();
  OCR1A = 0;  // IN1
  OCR0A = s;  // IN2
  OCR0B = 0;  // IN3
  OCR1B = s;  // IN4
  motorDirection = -1;
}
void motors_left() {
  int s = currentEffectiveSpeed();
  OCR1A = 0;  // IN1
  OCR0A = s;  // IN2
  OCR0B = s;  // IN3
  OCR1B = 0;  // IN4
  motorDirection = 2;
}
void motors_correctleft() {
  // Slight left turn - reduce left side speed
  int s = currentEffectiveSpeed();
  OCR1A = s - 30;  // IN1
  OCR0A = 0;       // IN2
  OCR0B = s;       // IN3
  OCR1B = 0;       // IN4
  motorDirection = 2;
}
void motors_right() {
  int s = currentEffectiveSpeed();
  OCR1A = s;  // IN1
  OCR0A = 0;  // IN2
  OCR0B = 0;  // IN3
  OCR1B = s;  // IN4
  motorDirection = 3;
}
void motors_correctright() {
  // Slight right turn - reduce right side speed
  int s = currentEffectiveSpeed();
  OCR1A = s;       // IN1
  OCR0A = 0;       // IN2
  OCR0B = 0;       // IN3
  OCR1B = s - 30;  // IN4
  motorDirection = 3;
}
void motors_brake() {
  OCR1A = 255;  // IN1
  OCR0A = 255;  // IN2
  OCR0B = 255;  // IN3
  OCR1B = 255;  // IN4
  motorDirection = 0;
}
void motors_coast() {
  OCR1A = 0;  // IN1
  OCR0A = 0;  // IN2
  OCR0B = 0;  // IN3
  OCR1B = 0;  // IN4
  motorDirection = 0;
}
