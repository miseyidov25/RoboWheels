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
      case 'F': motors_set_speed(185); motors_forward(); break;
      case 'B': motors_set_speed(185); motors_reverse(); break;
      case 'L': motors_set_speed(185); motors_left();    break;
      case 'R': motors_set_speed(185); motors_right();   break;
      case 'S': motors_set_speed(185); motors_brake();   break;
      case 'C': motors_set_speed(185); motors_coast();   break;
      default:  /* ignore */      break;
    }
  }
}