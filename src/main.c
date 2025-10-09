#include <Arduino.h>
#include "motors.h"
#include "buttons.h"

void setup() {
    motors_init();        // Initialize motor pins
    buttons_init();       // Initialize test button
}

void loop() {
    buttons_update();     // Update button state (handle debounce & toggle)

    // Move robot based on toggle
    if (is_test_forward_active()) {
        motors_set_speed(185); // Adjust speed as desired
        motors_forward();
    } else {
        motors_coast();
    }

    delay(30); // Small delay for stability
}
