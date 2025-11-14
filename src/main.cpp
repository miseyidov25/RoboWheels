#include <Arduino.h>
#include "motors.h"
#include "buttons.h"
#include "echo.h"
#include "leds.h"
#include "bt.h"

// Define pins for ultrasonic sensor
const int TRIG_PIN = 8;
const int ECHO_PIN = 7;
const int STOP_DISTANCE_CM = 10;  // Stop if object closer than 10 cm

void setup() {
    motors_init();        // Initialize motor pins
    buttons_init();       // Initialize test button
    echo_init(TRIG_PIN, ECHO_PIN); // Ultrasonic sensor
    bt_init();      // Initialize Bluetooth module
}

void loop() {
    buttons_update();     // Update button state (handle debounce & toggle)
    bt_update(); // Update Bluetooth commands
    
    unsigned long now = millis();  // Get current time
    int distance = echo_getDistance();
    bool obstacle = (distance > 0 && distance <= STOP_DISTANCE_CM);

    if (obstacle) {
        motors_coast();
        motors_reverse();
        delay(100);
        motors_coast();
    }

    // --- LED control ---
    leds_update(now, is_test_forward_active(), false, false, false);
}
