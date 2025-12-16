#include "leds.h"
#include "motors.h"
#include <avr/io.h>
#include "pins.h"

void leds_init() {
    // A0 = PC0, A1 = PC1 → OUTPUT
    DDRC |= (1 << PC0) | (1 << PC1);
}

void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right) {
    // Turn both LEDs OFF
    PORTC &= ~((1 << PC0) | (1 << PC1));

    // Left indicator
    if (motorDirection == 2 || left) {
        PORTC |= (1 << PC0);
    }

    // Right indicator
    if (motorDirection == 3 || right) {
        PORTC |= (1 << PC1);
    }

    // Brake / reverse
    if (motorDirection == -1 || rev) {
        PORTC |= (1 << PC0) | (1 << PC1);
    }
}
