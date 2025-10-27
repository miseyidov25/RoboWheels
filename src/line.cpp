#include <Arduino.h>
#include "motors.h"
#include "buttons.h"
#include "line.h"

#define left 12
#define right 10
#define middle 11
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

  if(leftSensor == 0 && middleSensor == 0 && rightSensor == 0){  // All sensors on line - on
    digitalWrite(LINE_LED, HIGH);
  }
  else if(leftSensor == 1 && middleSensor == 1 && rightSensor == 1){  // All sensors off line - off
    digitalWrite(LINE_LED, LOW);
  }
  else if(leftSensor == 0 && middleSensor == 0 && rightSensor == 1){  // Right sensor off line - blink (100ms)
    if(currentMillis - previousMillis >= 100){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
  }
  else if(leftSensor == 1 && middleSensor == 0 && rightSensor == 0){  // Left sensor off line - blink (500ms)
    if(currentMillis - previousMillis >= 500){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
  }
  else if(leftSensor == 1 && middleSensor == 0 && rightSensor == 1){  // Only middle on line - blink (300ms)
    if(currentMillis - previousMillis >= 300){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
  }
  else if(leftSensor == 0 && middleSensor == 1 && rightSensor == 1){  // Only left on line - blink (200ms)
    if(currentMillis - previousMillis >= 200){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
  }
  else if(leftSensor == 1 && middleSensor == 1 && rightSensor == 0){  // Only right on line - blink (400ms)
    if(currentMillis - previousMillis >= 400){
      previousMillis = currentMillis;
      ledState = !ledState;
      digitalWrite(LINE_LED, ledState);
    }
  }
}