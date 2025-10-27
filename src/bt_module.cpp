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
      case 'F': motors_forward(); break;
      case 'B': motors_reverse(); break;
      case 'L': motors_left();    break;
      case 'R': motors_right();   break;
      case 'S': motors_brake();   break;
      case 'C': motors_coast();   break;
      default:  /* ignore */      break;
    }
  }
}