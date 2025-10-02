#include <Arduino.h>
#include "motors.h"
#include "leds.h"
#include "buttons.h"
#include "echo.h"

// Pins
#define TRIG 8
#define ECHO 7
#define STOP_DISTANCE 10

void setup() {
    motors_init();
    leds_init();
    buttons_init();
    echo_init(TRIG, ECHO);
}

void loop() {
    buttons_update();
    unsigned long now = millis();

    bool fwd = is_forward_pressed();
    bool rev = is_reverse_pressed();
    bool left = is_left_pressed();
    bool right = is_right_pressed();

    int distance = echo_getDistance();
    bool obstacle = (distance > 0 && distance <= STOP_DISTANCE);

    motors_update(fwd, rev, left, right, obstacle);
    leds_update(now, fwd, rev, left, right);
}
