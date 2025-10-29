#include "leds.h"
#include <Arduino.h>

// Turn signal LED pins (moved away from motor PWM pins)
#define LED_TURN_L 2
#define LED_TURN_R 3

// RGB LED pins
#define LED_RGB_R 11
#define LED_RGB_G 12
#define LED_RGB_B 13

#define BLINK_INTERVAL 300

void leds_init() {
    // RGB LED pins
    pinMode(LED_RGB_R, OUTPUT);
    pinMode(LED_RGB_G, OUTPUT);
    pinMode(LED_RGB_B, OUTPUT);
    // Turn signal LEDs
    pinMode(LED_TURN_L, OUTPUT);
    pinMode(LED_TURN_R, OUTPUT);
}

// Update LED states based on input and current mode
// mode: 0 = Slave, 1 = Autonomous, 2 = Control
void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right, int mode) {
    // Turn all LEDs off by default
    digitalWrite(LED_TURN_L, LOW);
    digitalWrite(LED_TURN_R, LOW);
    digitalWrite(LED_RGB_R, LOW);
    digitalWrite(LED_RGB_G, LOW);
    digitalWrite(LED_RGB_B, LOW);

    // Left turn signal: light continuously while turning left
    if (left) {
        digitalWrite(LED_TURN_L, HIGH);
    }

    // Right turn signal: light continuously while turning right
    if (right) {
        digitalWrite(LED_TURN_R, HIGH);
    }

    // Brake: both turn LEDs on
    if (rev) {
        digitalWrite(LED_TURN_L, HIGH);
        digitalWrite(LED_TURN_R, HIGH);
    }

    // Set RGB color based on mode 
    switch (mode) {
        case 0: // Slave -> Blue
            digitalWrite(LED_RGB_B, HIGH);
            break;
        case 1: // Autonomous -> Green
            digitalWrite(LED_RGB_G, HIGH);
            break;
        case 2: // Control -> Red
            digitalWrite(LED_RGB_R, HIGH);
            break;
        default: // Unknown mode -> white
            digitalWrite(LED_RGB_R, HIGH);
            digitalWrite(LED_RGB_G, HIGH);
            digitalWrite(LED_RGB_B, HIGH);
            break;
    }
}
