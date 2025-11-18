#include <Arduino.h>
#include "bt.h"
#include "motors.h"

// NOTE: Serial.begin is done in main.setup() — don't initialize it again here.

void bt_init() {
  // nothing here; keep Serial.begin in main
}

void bt_update() {
  if (Serial.available()) {
    char cmd = Serial.read();
    Serial.print("Got: ");
    Serial.println(cmd);

    switch (cmd) {
      // Movement
      case 'F': motors_forward(); break;
      case 'B': motors_reverse(); break;
      case 'L': motors_left();    break;
      case 'R': motors_right();   break;
      case 'S': motors_brake();   break;
      case 'C': motors_coast();   break;

      // Speed control (set index)
      case '1': currentSpeedIndex = 0; break;
      case '2': currentSpeedIndex = 1; break;
      case '3': currentSpeedIndex = 2; break;
      case '4': currentSpeedIndex = 3; break;

      default: break;
    }

    // bounds-check and apply speed safely
    if (currentSpeedIndex < 0) currentSpeedIndex = 0;
    if (currentSpeedIndex >= speedLevelsCount) currentSpeedIndex = speedLevelsCount - 1;

    motors_set_speed(speedLevels[currentSpeedIndex]);

    // optional feedback
    Serial.print("Speed set to: ");
    Serial.println(speedLevels[currentSpeedIndex]);
  }
}
