#include <Arduino.h>
#include "bt.h"

// Motor pins
#define IN1 7;
#define IN2 6;
#define IN3 4:
#define IN4 2;
#define ENA 3;
#define ENB 5;

void setup() {
  Serial.begin(9600);

  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);
  pinMode(ENB, OUTPUT);

  analogWrite(ENA, 255);
  analogWrite(ENB, 255);
  motors_coast();
}

void loop() {
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