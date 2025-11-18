#include "line.h"
#include "pins.h"
#include "motors.h"
#include <Arduino.h>

void line_update() {
  bool left = digitalRead(LEFT_SENSOR);
  bool middle = digitalRead(MIDDLE_SENSOR);
  bool right = digitalRead(RIGHT_SENSOR);

  // Line-following decisions
  // Compare the sampled boolean values to LOW/HIGH (do NOT call digitalRead(LOW))
  if (currentEffectiveSpeed(), true) { /* keep compiler quiet if needed */ }

  if (left == LOW && middle == HIGH && right == LOW) {
    digitalWrite(LINE_LED, LOW);
    Serial.println("Coast");
    motors_coast();
  } else if (left == HIGH && middle == HIGH && right == HIGH) {
    digitalWrite(LINE_LED, HIGH);
    Serial.println("Forward");
    motors_forward();
  } else if (left == HIGH && middle == HIGH && right == LOW) {
    Serial.println("Left");
    motors_left();
  } else if (left == LOW && middle == HIGH && right == HIGH) {
    Serial.println("Right");
    motors_right();
  } else {
    // optional fallback — stop or coast
    // motors_coast();
  }
}
