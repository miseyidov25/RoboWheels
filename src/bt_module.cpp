#include <Arduino.h>
#include "bt.h"
#include "motors.h"

// Use speedLevels/currentSpeedIndex defined in motors.cpp (declared extern in motors.h)

void bt_init() {
    // Serial.begin is in main.cpp
}

// Use THE SAME global variable defined in main.cpp:
extern Mode currentMode;

void bt_update() {
    if (!Serial.available()) return;

    char cmd = Serial.read();
    Serial.print("Got: ");
    Serial.println(cmd);

    bool speedChanged = false;

    switch (cmd) {
        case 'F': motors_forward(); break;
        case 'B': motors_reverse(); break;
        case 'L': motors_left();    break;
        case 'R': motors_right();   break;
        case 'S': motors_coast();   break;
        case 'C': motors_coast();   break;
        case 'H': motors_correctright();  break;
        case 'G': motors_correctleft();   break;

        case 'W': currentMode = AUTONOMOUS; break;
        case 'X': currentMode = SLAVE;      break;
        case 'w': currentMode = MANUAL;     break;
        case 'x': currentMode = MANUAL;     break;

        case '1': currentSpeedIndex = 0; speedChanged = true; break;
        case '2': currentSpeedIndex = 1; speedChanged = true; break;
        case '3': currentSpeedIndex = 2; speedChanged = true; break;
        case '4': currentSpeedIndex = 3; speedChanged = true; break;

        default: break;
    }

    // Keep index in range
    if (currentSpeedIndex < 0) currentSpeedIndex = 0;
    if (currentSpeedIndex >= speedLevelsCount)
        currentSpeedIndex = speedLevelsCount - 1;

    if (speedChanged) {
        motors_set_speed(speedLevels[currentSpeedIndex]);
        Serial.print("Speed set to: ");
        Serial.println(speedLevels[currentSpeedIndex]);
    }
}
