#include <Arduino.h>
#include "motors.h"
#include "buttons.h"
#include "line.h"

#define left 13
#define right 11
#define middle 12
#define LINE_LED 13

static unsigned long previousMillis = 0;
static int ledState = LOW;

void line_init() {
  pinMode(left,INPUT);
  pinMode(right,INPUT);
  pinMode(middle,INPUT);
  pinMode(LINE_LED,OUTPUT);
}

void line_update(){
  bool leftSensor = digitalRead(left);
  bool middleSensor = digitalRead(middle);
  bool rightSensor = digitalRead(right);
  unsigned long currentMillis = millis();

  if(leftSensor == 0 && middleSensor == 0 && rightSensor == 0){  // All sensors on line - forward
    digitalWrite(LINE_LED, HIGH);
    motors_forward();
  }
  else if(leftSensor == 1 && middleSensor == 1 && rightSensor == 1){  // All sensors off line - stop
    digitalWrite(LINE_LED, LOW);
    motors_brake();
  }
  else if(leftSensor == 0 && middleSensor == 0 && rightSensor == 1){  // Right sensor off line - turn rleft
    if(currentMillis - previousMillis >= 100){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
    motors_left();
  }
  else if(leftSensor == 1 && middleSensor == 0 && rightSensor == 0){  // Left sensor off line - turn right
    if(currentMillis - previousMillis >= 500){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
    motors_right();
  }
  else if(leftSensor == 1 && middleSensor == 0 && rightSensor == 1){  // Only middle on line - forward
    if(currentMillis - previousMillis >= 300){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
    motors_forward();
  }
  else if(leftSensor == 0 && middleSensor == 1 && rightSensor == 1){  // Only left on line - turn rlef
    if(currentMillis - previousMillis >= 200){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
    motors_left();
  }
  else if(leftSensor == 1 && middleSensor == 1 && rightSensor == 0){  // Only right on line - turn 
    if(currentMillis - previousMillis >= 400){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
    motors_right();
  }
}