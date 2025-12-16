#include "line.h"
#include "pins.h"
#include "motors.h"
#include <Arduino.h>

void line_update() {
  
  bool left   = !(PIND & (1 << PD4));
  bool middle = !(PIND & (1 << PD3));
  bool right  = !(PIND & (1 << PD2));

  // Line-following decisions
  // Compare the sampled boolean values to LOW/HIGH (do NOT call digitalRead(LOW))
  if (currentEffectiveSpeed(), true) {}

 //LOW = on line, HIGH = off line

  if (left == LOW && middle == HIGH && right == LOW) {
    Serial.println("Forward");
    motors_forward();
  } else if (left == LOW && middle == LOW && right == LOW) {
    Serial.println("All high");
    motors_coast();
  } else if (left == LOW && middle == LOW && right == HIGH) {
    Serial.println("Right");
    motors_right();
  } else if (left == LOW && middle == HIGH && right == HIGH) {
    Serial.println("Right & middle");
    motors_forward();
  }else if (left == HIGH && middle == LOW && right == LOW) {
    Serial.println("Left");
    motors_left();
  } else if (left == HIGH && middle == HIGH && right == LOW) {
    Serial.println("Left & middle");
    motors_forward();
  }else if (left == HIGH && middle == HIGH && right == HIGH) {
    Serial.println("All low");
    motors_coast();
  }else {
    Serial.println("Something wrong");
    motors_coast();
  }
}
