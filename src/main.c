#include <Arduino.h>
#include "motors.h"
#include "buttons.h"
#include "bt.h"

// LED pins
#define LED_L 9
#define LED_R 10
#define LED_B 11

#define BLINK_INTERVAL 300

void leds_init() {
    pinMode(LED_L, OUTPUT);
    pinMode(LED_R, OUTPUT);
    pinMode(LED_B, OUTPUT);
}

void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right) {
    // default off
    digitalWrite(LED_L, LOW);
    digitalWrite(LED_R, LOW);
    digitalWrite(LED_B, LOW);

    if (left) {
        if ((now / BLINK_INTERVAL) % 2 == 0) digitalWrite(LED_L, HIGH);
    }
    if (right) {
        if ((now / BLINK_INTERVAL) % 2 == 0) digitalWrite(LED_R, HIGH);
    }
    if (rev) {
        digitalWrite(LED_B, HIGH);
    }
}

void setup() {
    motors_init();
    buttons_init();
    leds_init();
}

void loop() {
    buttons_update();
    unsigned long now = millis();

    // --- Motor control ---
    if (is_forward_pressed() && !is_reverse_pressed()) motors_forward();
    else if (is_reverse_pressed() && !is_forward_pressed()) motors_reverse();
    else if (is_forward_pressed() && is_reverse_pressed()) motors_brake();
    else motors_coast();

    if (is_left_pressed() && !is_right_pressed()) motors_right();
    else if (is_right_pressed() && !is_left_pressed()) motors_left();
    else if (is_left_pressed() && is_right_pressed()) motors_brake();
    else motors_coast();

    // --- LED control ---
    leds_update(now, is_forward_pressed(), is_reverse_pressed(), is_left_pressed(), is_right_pressed());
}
