#include <Arduino.h>
#include "motors.h"
#include "echo.h"
#include "leds.h"
#include "bt.h"

// Define pins for ultrasonic sensor
const int TRIG_PIN = 8;
const int ECHO_PIN = 7;
const int ECHO_PIN2 = 2;
const int ECHO_PIN3 = 3;
const int ECHO_PIN4 = 4;
const int STOP_DISTANCE_CM = 10;  // Stop if object closer than 10 cm

void setup() {
    motors_init();        // Initialize motor pins
    echo_init(TRIG_PIN, ECHO_PIN, ECHO_PIN2, ECHO_PIN3, ECHO_PIN4); // Ultrasonic sensor
    bt_init();      // Initialize Bluetooth module
}

void loop() {
    
    bt_update(); // Update Bluetooth commands
    
    unsigned long now = millis();  // Get current time
    
    echo_update();        // Read ultrasonic sensor and handle obstacles

    
    // --- LED control ---
    // leds_update(now, is_test_forward_active(), false, false, false);
}
