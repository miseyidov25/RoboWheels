#include "line.h"
#include "pins.h"
#include "motors.h"
#include <Arduino.h>
#include <stdio.h>

extern int currentSpeedIndex;
extern const int speedLevels[];
extern const int speedLevelsCount;

static int allHighCounter = 0;
static bool speedBoosted = false;
static unsigned long boostStartTime = 0;

void line_update() {
  
  bool left   = !(PIND & (1 << PD4));
  bool middle = !(PIND & (1 << PD3));
  bool right  = !(PIND & (1 << PD2));

  bool allHigh = (left == HIGH && middle == HIGH && right == HIGH);

  if (allHigh) {
    allHighCounter++;
    if (allHighCounter >= 3 && !speedBoosted) {
      speedBoosted = true;
      boostStartTime = millis();
      int nextIndex = currentSpeedIndex + 1;
      if (nextIndex >= speedLevelsCount) nextIndex = speedLevelsCount - 1;
      motors_set_speed(speedLevels[nextIndex]);
      printf("Speed boosted due to all high\n");
    }
  } else {
    allHighCounter = 0;
  }

  // Check for boost timeout
  if (speedBoosted && (millis() - boostStartTime > 2000)) {
    speedBoosted = false;
    motors_set_speed(speedLevels[currentSpeedIndex]);
    printf("Speed reverted after timeout\n");
  }

  // Line-following decisions
  // Compare the sampled boolean values to LOW/HIGH (do NOT call digitalRead(LOW))
  if (currentEffectiveSpeed(), true) {}

 //LOW = on line, HIGH = off line

  if (left == HIGH && middle == LOW && right == HIGH) {
    printf("Forward\n");
    motors_forward();
  } else if (left == HIGH && middle == LOW && right == LOW) {
    printf("Right & middle\n");
    motors_forward();
  }else if (left == HIGH && middle == HIGH && right == LOW) {
    printf("Right\n");
    motors_right();
  } else if (left == LOW && middle == LOW && right == HIGH) {
    printf("Left & middle\n");
    motors_forward();
  }else if (left == LOW && middle == HIGH && right == HIGH) {
    printf("Left\n");
    motors_left();
  } else if (left == LOW && middle == LOW && right == LOW) {
    printf("All low\n");
    motors_right();
  }else if (left == HIGH && middle == HIGH && right == HIGH) {
    printf("All high\n");
    motors_reverse();
  } else {
    printf("Something wrong\n");
    motors_coast();
  }
}
