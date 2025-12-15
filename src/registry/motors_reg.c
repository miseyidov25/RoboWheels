#include "motors.h"

#include <stdint.h>
#include <stdbool.h>
#include <avr/io.h>

// Speed levels for Bluetooth control
int currentSpeedIndex = 2;  // default = f3 (185)


volatile motor_direction_t motorDirection = MOTOR_STOP;

int motorSpeed = 220;      // only if you want default here
int motorSpeedAuto = 110;    // define it somewhere

void motors_pwm_sync_init(void)
{
    /* Disable timers */
    TCCR0B = 0;
    TCCR1B = 0;

    /* Reset counters */
    TCNT0 = 0;
    TCNT1 = 0;

    /* Timer0: Fast PWM */
    TCCR0A =
        (1 << WGM00) | (1 << WGM01) |
        (1 << COM0A1) |
        (1 << COM0B1);

    /* Timer1: Fast PWM 8-bit */
    TCCR1A =
        (1 << WGM10) |
        (1 << COM1A1) |
        (1 << COM1B1);

    /* Start timers simultaneously (prescaler = 64) */
    TCCR0B = (1 << CS01) | (1 << CS00);
    TCCR1B = (1 << WGM12) | (1 << CS11) | (1 << CS10);
}


void motors_init(void)
{
    /* Configure motor pins as OUTPUT */
    DDRB |= (1 << PB1) | (1 << PB2);   // IN1 (PB1), IN4 (PB2)
    DDRD |= (1 << PD5) | (1 << PD6);   // IN3 (PD5), IN2 (PD6)

    /*  Initialize and synchronize PWM timers */
    motors_pwm_sync_init();

    /*  Ensure motors are stopped */
    motors_coast();

    /* Default state */
    motorSpeed = 120;
}
static inline int constrain_int(int val, int min, int max) {
    if (val < min) return min;
    if (val > max) return max;
    return val;
}

void motors_set_speed(int speed) {
    motorSpeed = constrain_int(speed, 0, 255);  
}

void motors_set_speed_ramp(uint8_t speed) { // TODO: test it
    if (speed > motorSpeed) motorSpeed++;
    else if (speed < motorSpeed) motorSpeed--;
}

void motors_forward(void)
{
    OCR1A = motorSpeed;  // IN1
    OCR0A = 0;           // IN2

    OCR0B = motorSpeed;  // IN3
    OCR1B = 0;           // IN4

    motorDirection = MOTOR_FORWARD;
}


void motors_reverse(void)
{
    OCR1A = 0;
    OCR0A = motorSpeed;

    OCR0B = 0;
    OCR1B = motorSpeed;

    motorDirection = MOTOR_REVERSE;
}


void motors_left(void)
{
    OCR1A = 0;
    OCR0A = motorSpeed;

    OCR0B = motorSpeed;
    OCR1B = 0;

    motorDirection = MOTOR_LEFT;
}


void motors_correctleft(void)
{
    OCR1A = 0;
    OCR0A = motorSpeed;

    OCR0B = motorSpeed - 30;
    OCR1B = 0;

    motorDirection = MOTOR_CORRECT_LEFT;
}


void motors_right(void)
{
    OCR1A = motorSpeed;
    OCR0A = 0;

    OCR0B = 0;
    OCR1B = motorSpeed;

    motorDirection = MOTOR_RIGHT;
}


void motors_correctright(void)
{
    OCR1A = motorSpeed;
    OCR0A = 0;

    OCR0B = 0;
    OCR1B = motorSpeed - 30;

    motorDirection = MOTOR_CORRECT_RIGHT;
}


void motors_coast(void)
{
    OCR1A = 0;
    OCR0A = 0;
    OCR0B = 0;
    OCR1B = 0;

    motorDirection = MOTOR_STOP;
}


void motors_update(bool fwd) {
    if (fwd) motors_forward();
    else motors_coast();
}
