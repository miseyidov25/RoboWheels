#include "leds.h"
#include "motors.h"
#include <avr/io.h>
#include "pins.h"

void leds_init(void) {
    // A0 = PC0, A1 = PC1 → OUTPUT
    DDRC |= (1 << PC0) | (1 << PC1);
}

void leds_update(void) {
    // Turn both LEDs OFF
    PORTC &= ~((1 << PC0) | (1 << PC1));

    // Left indicator
    if (motorDirection == MOTOR_LEFT || motorDirection == MOTOR_CORRECT_LEFT) {
        PORTC |= (1 << PC0);
    }

    // Right indicator
    if (motorDirection == MOTOR_RIGHT || motorDirection == MOTOR_CORRECT_RIGHT) {
        PORTC |= (1 << PC1);
    }

    // Reverse
    if (motorDirection == MOTOR_REVERSE) {
        PORTC |= (1 << PC0) | (1 << PC1);
    }
}
