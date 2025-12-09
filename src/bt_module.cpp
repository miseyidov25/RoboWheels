#include <Arduino.h>
#include "bt.h"
#include "motors.h"

// External globals
extern Mode currentMode;
extern bool menuActive;
extern bool hasSelectedMode;
extern int currentSpeedIndex;
extern int speedLevels[];
extern const int speedLevelsCount;

// Track active command and previous state
static char activeCmd = '\0';
static char prevCmd = '\0';
static unsigned long lastCmdTime = 0;  // millis() of last command
const unsigned long STOP_DELAY = 500;  // 500 ms stop delay

void bt_init() {
    // Serial already initialized in main.cpp
}

void executeCommand(char cmd) {
    // Lowercase mode letters go to menu
    if (cmd >= 'a' && cmd <= 'z') {
        if (cmd == 'w' || cmd == 'x' || cmd == 'u') {
            currentMode = NONE; // return to menu
            menuActive = true;
            hasSelectedMode = false;
            Serial.println("Returning to menu...");
            return;
        } else {
            cmd -= 32;  // Normalize other lowercase letters
        }
    }

    // Speed commands
    if (cmd >= '1' && cmd <= '4') {
        currentSpeedIndex = cmd - '1';
        if (currentSpeedIndex < 0) currentSpeedIndex = 0;
        if (currentSpeedIndex >= speedLevelsCount) currentSpeedIndex = speedLevelsCount - 1;
        motors_set_speed(speedLevels[currentSpeedIndex]);
        Serial.print("Speed: ");
        Serial.println(speedLevels[currentSpeedIndex]);
        return;
    }

    // Mode commands
    switch (cmd) {
        case 'W': currentMode = AUTONOMOUS; menuActive = false; hasSelectedMode = true; Serial.println("Mode: AUTONOMOUS"); return;
        case 'X': currentMode = SLAVE;      menuActive = false; hasSelectedMode = true; Serial.println("Mode: SLAVE");      return;
        case 'U': currentMode = MANUAL;     menuActive = false; hasSelectedMode = true; Serial.println("Mode: MANUAL");     return;
    }

    // Movement commands only in MANUAL
    if (currentMode != MANUAL) {
        Serial.print("Ignored command '"); Serial.print(cmd); Serial.println("' - not in MANUAL mode");
        return;
    }

    // Update active command
    activeCmd = cmd;
    lastCmdTime = millis(); // reset timer on every command
}

void bt_update() {
    bool gotCmd = false;

    // Read all serial input
    while (Serial.available()) {
        char cmd = Serial.read();
        if (cmd == '\r' || cmd == '\n' || cmd == ' ' || cmd == '\0') continue;
        executeCommand(cmd);
        gotCmd = true;
    }

    unsigned long now = millis();

    // If command changed, start new movement immediately
    if (activeCmd != prevCmd && activeCmd != '\0') {
        switch (activeCmd) {
            case 'F': motors_forward();      Serial.println("Got: F (Forward)"); break;
            case 'B': motors_reverse();      Serial.println("Got: B (Reverse)"); break;
            case 'L': motors_left();         Serial.println("Got: L (Left)"); break;
            case 'R': motors_right();        Serial.println("Got: R (Right)"); break;
            case 'H': motors_correctright(); Serial.println("Got: H (Correct Right)"); break;
            case 'G': motors_correctleft();  Serial.println("Got: G (Correct Left)"); break;
            case 'S':
            case 'C': motors_coast();        Serial.println("Got: S/C (Stop/Coast)"); break;
            default:  Serial.print("Unknown command: "); Serial.println(activeCmd); break;
        }
        prevCmd = activeCmd;
    }

    // Send stop only if no command received for STOP_DELAY
    if (activeCmd != '\0' && (now - lastCmdTime > STOP_DELAY)) {
        motors_coast();
        Serial.println("Got: S/C (Stop/Coast)");
        activeCmd = '\0';
        prevCmd = '\0';
    }

    // If no serial input, don't overwrite activeCmd until STOP_DELAY
    if (!gotCmd && activeCmd == '\0') {
        prevCmd = '\0';
    }
}
