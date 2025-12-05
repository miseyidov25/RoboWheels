#include <Arduino.h>
#include "bt.h"
#include "motors.h"

// Use speedLevels/currentSpeedIndex defined in motors.cpp (declared extern in motors.h)

// Track last command and timeout for auto-stop
static char lastCmd = '\0';
static unsigned long lastCmdTime = 0;
const unsigned long CMD_TIMEOUT = 100;  // 100ms timeout for auto-stop

void bt_init() {
    // Serial.begin is in main.cpp
}

// Use THE SAME global variable defined in main.cpp:
extern Mode currentMode;

// External reference to OLED state
extern bool menuActive;
extern bool hasSelectedMode;

void bt_update() {
    unsigned long now = millis();

    // Auto-stop if no command received for CMD_TIMEOUT ms
    if (lastCmd != '\0' && (now - lastCmdTime) > CMD_TIMEOUT) {
        lastCmd = '\0';
        motors_coast();
        // Don't spam serial with auto-stop messages
    }

    if (!Serial.available()) return;

    char cmd = Serial.read();
    
    // Ignore empty/whitespace characters
    if (cmd == '\0' || cmd == '\r' || cmd == '\n' || cmd == ' ') {
        return;
    }
    
    // Convert lowercase to uppercase for consistency
    if (cmd >= 'a' && cmd <= 'z') {
        cmd = cmd - 'a' + 'A';
    }
    
    // Only execute and print if command changed
    if (cmd != lastCmd) {
        Serial.print("Got: ");
        Serial.println(cmd);
        lastCmd = cmd;
        lastCmdTime = now;
        
        // Execute the command
        executeCommand(cmd);
    } else {
        // Same command repeated, just update timestamp to keep it alive
        lastCmdTime = now;
    }
}

void executeCommand(char cmd) {
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

        case 'W': 
            currentMode = AUTONOMOUS;
            menuActive = false;
            hasSelectedMode = true;
            Serial.println("Mode set via BT: AUTONOMOUS");
            break;
        case 'X': 
            currentMode = SLAVE;
            menuActive = false;
            hasSelectedMode = true;
            Serial.println("Mode set via BT: SLAVE");
            break;
        case 'U': 
            currentMode = MANUAL;
            menuActive = false;
            hasSelectedMode = true;
            Serial.println("Mode set via BT: MANUAL");
            break;

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
