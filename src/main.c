#include <Arduino.h>
#include "motors.h"
#include "leds.h"
#include "buttons.h"
#include "echo.h"
#include "menu.h"

#define TRIG 10
#define ECHO 11
#define STOP_DISTANCE 10  

void setup() {

    motors_init();
    leds_init();
    buttons_init();
    echo_init(TRIG, ECHO);
    menu_init();

}

void loop() {
    unsigned long now = millis();

    buttons_update();
    menu_update();

    //Read the current mode from the menu 0 = Slave, 1 = Autonomous, 2 = Control
    int mode = menu_get_mode();  

    //Default driving signals
    bool fwd   = is_forward_pressed();
    bool rev   = is_reverse_pressed();
    bool left  = is_left_pressed();
    bool right = is_right_pressed();

    //Test button
    if (is_test_forward_active()) {
        fwd = true;
        rev = false;
    }

    //Measure distance using the ultrasonic sensor
    int distance = echo_getDistance();
    bool obstacle = (distance > 0 && distance <= STOP_DISTANCE);

    //Modes
    switch (mode) {
        case 0: //Slave Mode
            // Line following code

            break;

        case 1: //Autonomous Mode
            // Distance sensor code
            if (obstacle) {
                fwd = false;
                rev = false;
        
            } else {
                fwd = true;
                rev = false;
            }
            break;

        case 2: //Control Mode
            // Bluetooth control
           
            break;

        default:
          
            break;
    }

    motors_update(fwd, rev, left, right, obstacle);
    leds_update(now, fwd, rev, left, right);

}
