#include <Arduino.h>
#include <Wire.h>
#include "pins.h"
#include "motors.h"
#include "line.h"
#include "bt.h"
#include "echo.h"
#include "oled.h"


// GLOBAL MODE VARIABLE DEFINED HERE 
Mode currentMode = NONE;

// Speed Control
int motorSpeed = 230;
int motorSpeedAuto = 110;

unsigned long previousMillis = 0;

// make it available to other files:
int currentEffectiveSpeed() {
    return (currentMode == AUTONOMOUS) ? motorSpeedAuto : motorSpeed;
}

void setup() {
    Serial.begin(9600);

    // Initialize I2C for OLED display
    Wire.begin();

    motors_init();
    echo_init(TRIG_PIN, ECHO_PIN1, ECHO_PIN2,ECHO_PIN3,ECHO_PIN4);  

    pinMode(LEFT_SENSOR, INPUT);
    pinMode(MIDDLE_SENSOR, INPUT);
    pinMode(RIGHT_SENSOR, INPUT);
    pinMode(LINE_LED, OUTPUT);

    oled_init(); 
    bt_init();
}

void loop() {
    oled_update();  
    bt_update();    

    if (!menuActive) {
        // Autonomous mode
        if (currentMode == AUTONOMOUS) {
            echo_update();
        }
        // Slave mode (line follow)
        else if (currentMode == SLAVE) {
            line_update();
        }
        // Manual mode -> nothing here (Bluetooth handles it)
    }
}