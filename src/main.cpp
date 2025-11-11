#include <Arduino.h>
#include "motors.h"
#include "buttons.h"
#include "echo.h"
#include "leds.h"
#include "bt.h"
#include "line.h"

// Define pins for ultrasonic sensor
const int TRIG_PIN = 8;
const int ECHO_PIN = 7;
const int STOP_DISTANCE_CM = 10;  // Stop if object closer than 10 cm

void setup() {
    motors_init();        // Initialize motor pins
    buttons_init();       // Initialize test button
    line_init();          // Initialize line sensors
/*    leds_init();          // Initialize LEDs */
}

void loop() {
    buttons_update();     // Update button state (handle debounce & toggle)
    bt_update(); // Update Bluetooth commands
    
    unsigned long now = millis();  // Get current time
    int distance = echo_getDistance();
    bool obstacle = (distance > 0 && distance <= STOP_DISTANCE_CM);


    // If the test button is ON, drive forward
    if (is_test_forward_active()) {
        if (obstacle) {
            motors_brake();
        } else {
            motors_set_speed(185);
            motors_forward();
        }
    } 
    // Otherwise, use line-following mode
    else {
        if (obstacle) {
            motors_brake();
        } else {
            line_update();
        }
    }
}
