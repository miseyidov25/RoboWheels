#include <Arduino.h>
#include "pins.h"
#include "motors.h"
#include "line.h"
#include "bt.h"
#include "echo.h"

// === GLOBAL MODE VARIABLE DEFINED HERE ===
Mode currentMode = MANUAL;

// === Speed Control ===
int motorSpeed = 230;
int motorSpeedAuto = 110;

unsigned long previousMillis = 0;

// make it available to other files:
int currentEffectiveSpeed() {
    return (currentMode == AUTONOMOUS) ? motorSpeedAuto : motorSpeed;
}

void setup() {
    Serial.begin(9600);

    motors_init();

    pinMode(LEFT_SENSOR, INPUT);
    pinMode(MIDDLE_SENSOR, INPUT);
    pinMode(RIGHT_SENSOR, INPUT);
    pinMode(LINE_LED, OUTPUT);

    bt_init();
}

void loop() {
    bt_update();

    // Autonomous
    if (currentMode == AUTONOMOUS) {
        echo_init(TRIG_PIN, ECHO_PIN);
        int distance_cm = echo_getDistance();

        if(distance_cm > 10) {
            motors_forward(); 
            Serial.println("Autonomous Forward");
        }
        else if(distance_cm <= 10) {
            motors_coast();
            Serial.println("Autonomous Brake");
        }
        return;
    }

    // Slave (line follow)
    if (currentMode == SLAVE) {
        line_update();
        return;
    }

    // Manual mode → nothing here (Bluetooth handles it)
}
