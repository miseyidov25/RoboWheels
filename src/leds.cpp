#include "leds.h"
#include "pins.h"

// LED pins
#define LED_L 9
#define LED_R 10
#define LED_B 11

// Blink interval for turn signals
#define BLINK_INTERVAL 300

void leds_init() {
    pinMode(LED_L, OUTPUT);
    pinMode(LED_R, OUTPUT);
    pinMode(LED_B, OUTPUT);
}

// Update LED states based on input
void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right) {
    // Turn all LEDs off by default
    digitalWrite(LED_L, LOW);
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_B, LOW);

    // Left turn signal
    if (left) {
        if ((now / BLINK_INTERVAL) % 2 == 0) digitalWrite(LED_L, HIGH);
    }

    // Right turn signal
    if (right) {
        if ((now / BLINK_INTERVAL) % 2 == 0) digitalWrite(LED_R, HIGH);
    }

    // Brake LED
    if (rev) {
        digitalWrite(LED_B, HIGH);
    }
}
