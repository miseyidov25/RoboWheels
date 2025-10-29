#include <Arduino.h>
#include "motors.h"
#include "buttons.h"
#include "line.h"

#define left 13
#define right 11
#define middle 12
#define LINE_LED 4

static unsigned long previousMillis = 0;
static int ledState = LOW;

void line_init() {
  pinMode(left,INPUT);
  pinMode(right,INPUT);
  pinMode(middle,INPUT);
  pinMode(LINE_LED,OUTPUT);
}

void line_update(){
  bool leftSensor = !digitalRead(left);
  bool middleSensor = !digitalRead(middle);
  bool rightSensor = !digitalRead(right);
  unsigned long currentMillis = millis();

  if(leftSensor == 0 && middleSensor == 1 && rightSensor == 0){  // Only middle on line - forward
    motors_forward();
  }
  else if(leftSensor == 0 && middleSensor == 0 && rightSensor == 0){  // All sensors off line - stop
    digitalWrite(LINE_LED, HIGH);
    motors_coast();
  }
  else if(leftSensor == 1 && middleSensor == 1 && rightSensor == 0){  // Right sensor off line - turn left
    motors_left();
  }
  else if(leftSensor == 0 && middleSensor == 1 && rightSensor == 1){  // Left sensor off line - turn right
    motors_right();
  }
  else if(leftSensor == 1 && middleSensor == 1 && rightSensor == 1){  // All sensors on line - forward
    digitalWrite(LINE_LED, HIGH);
    motors_forward();
  }
  else if(leftSensor == 1 && middleSensor == 0 && rightSensor == 0){  // Only left on line - turn left
    motors_left();
  }
  else if(leftSensor == 0 && middleSensor == 0 && rightSensor == 1){  // Only right on line - turn 
    motors_right();
  }
}