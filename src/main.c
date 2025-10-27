#include <Arduino.h>
#include "motors.h"
#include "buttons.h"
#include "line.h"

#define left 12
#define right 10
#define middle 11
#define LINE_LED 13

unsigned long previousMillis = 0;
int ledState = LOW;

void setup() {
  line_init();
  motors_init();

}

void loop(){
  line_update();
}