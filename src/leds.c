#include "leds.h"

// LED pins
#define LED_L A0
#define LED_R A1
#define LEDR 4  //red
#define LEDG 3  //green
#define LEDB 2  //blue


// Blink interval for turn signals
#define BLINK_INTERVAL 300

void leds_init() {
    pinMode(LED_L, OUTPUT);
    pinMode(LED_R, OUTPUT);
    pinMode(LEDR,  OUTPUT);              
    pinMode(LEDG, OUTPUT);
    pinMode(LEDB, OUTPUT);
}


// Update LED states based on input
void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right) {
    // Turn all LEDs off by default
    digitalWrite(LED_L, LOW);
    digitalWrite(LED_R, LOW);

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
        digitalWrite(LED_R, HIGH);
        digitalWrite(LED_L, HIGH);
    }


}
    void ledsRGB_update(bool robot_on, int mode) {
    
        if (!robot_on) {
            digitalWrite(LEDR, LOW);
            digitalWrite(LEDG, LOW);
            digitalWrite(LEDB, LOW);
            return;
        }
        if (robot_on) {
            digitalWrite(LEDR, HIGH);
            digitalWrite(LEDG, LOW);
            digitalWrite(LEDB, LOW);
            return;
        }
    
        switch (mode) {
        // Slave mode
        case 0: // Yellow 
            digitalWrite(LEDR, HIGH);
            digitalWrite(LEDG, HIGH);
            digitalWrite(LEDB, LOW);
            break;
    
        // Autonomous mode
        case 1: // Green 
            digitalWrite(LEDR, LOW);
            digitalWrite(LEDG, HIGH);
            digitalWrite(LEDB, LOW);
            break;
    
        // Bluetooth/control mode
        case 2: // Blue
            digitalWrite(LEDR, LOW);
            digitalWrite(LEDG, LOW);
            digitalWrite(LEDB, HIGH);
            break;
    
        default:
            // Unknown mode: turn LEDs off
            digitalWrite(LEDR, HIGH);
            digitalWrite(LEDG, HIGH);
            digitalWrite(LEDB, HIGH);
            break;
        }
    }
    