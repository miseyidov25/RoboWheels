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

    // Left turn signal
    if (motorDirection == 2 || left) {
        digitalWrite(LED_L, HIGH);
    }

    // Right turn signal
    if (motorDirection == 3 || right) {
        digitalWrite(LED_R, HIGH);
    }

    // Brake LED
    if (motorDirection == -1 || rev) {
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_L, HIGH);
    }
}



