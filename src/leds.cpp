#include "leds.h"
#include <Arduino.h>
#include "motors.h"

// LED pins
#define LED_L A0
#define LED_R A1

// Blink interval for turn signals
#define BLINK_INTERVAL 300

void leds_init() {
    pinMode(LED_L, OUTPUT);
    pinMode(LED_R, OUTPUT);
}

// Update LED states based on input
void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right) {
    // Turn all LEDs off by default
    digitalWrite(LED_L, LOW);
    digitalWrite(LED_R, LOW);

    /* Left turn indicator */
    if (motorDirection == MOTOR_LEFT ||
        motorDirection == MOTOR_CORRECT_LEFT ||
        left)
    {
        digitalWrite(LED_L, HIGH);
    }

    /* Right turn indicator */
    if (motorDirection == MOTOR_RIGHT ||
        motorDirection == MOTOR_CORRECT_RIGHT ||
        right)
    {
        digitalWrite(LED_R, HIGH);
    }

    /* Brake / reverse indicator */
    if (motorDirection == MOTOR_REVERSE || rev)
    {
        digitalWrite(LED_L, HIGH);
        digitalWrite(LED_R, HIGH);
    }
}



