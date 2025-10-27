#include <Arduino.h>
#include "motors.h"
#include "buttons.h"
#include "line.h"

void setup() {
  line_init();
  motors_init();

}

void loop(){
  line_update();
}