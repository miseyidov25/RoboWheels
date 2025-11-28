#include <Arduino.h>
#include "bt.h"
#include "motors.h"

void bt_init() {
  Serial.begin(9600);
  
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
      case 'S': motors_coast();   break;
      case 'C': motors_coast();   break;

      // Speed control
      case '1': currentSpeedIndex = 0; break; // f1
      case '2': currentSpeedIndex = 1; break; // f2
      case '3': currentSpeedIndex = 2; break; // f3
      case '4': currentSpeedIndex = 3; break; // f4

      default: break;
    }

    motors_set_speed(speedLevels[currentSpeedIndex]);
  }
}