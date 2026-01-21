#include "line.h"
#include "pins.h"
#include "motors.h"
#include "echo.h"
#include <stdio.h>

extern int currentSpeedIndex;
extern const int speedLevels[];
extern const int speedLevelsCount;

static int allHighCounter = 0;
static bool speedBoosted = false;
static unsigned long boostStartTime = 0;
static unsigned long lastUpdateTime = 0;

void line_update() {
  unsigned long currentTime = millis();
  if (currentTime - lastUpdateTime < 30) {
    return; 
  }
  lastUpdateTime = currentTime;
  
  bool left   = !(PIND & (1 << PD4));
  bool middle = !(PIND & (1 << PD3));
  bool right  = !(PIND & (1 << PD2));

  bool allHigh = (left == HIGH && middle == HIGH && right == HIGH);

  typedef enum {
      DIR_LEFT,
      DIR_RIGHT,
      DIR_STRAIGHT
  } Direction;

  Direction lastDirection = DIR_STRAIGHT;

  // Get front distance
  int distance = echo_getDistance(0);

  // Determine effective speed
  int effectiveSpeed = speedLevels[currentSpeedIndex];
  bool shouldStop = false;

  if (distance <= 10) {
    shouldStop = true;
    printf("Obstacle too close (%d cm), stopping\n", distance);
  } else if (distance <= 20) {
    effectiveSpeed = speedLevels[currentSpeedIndex] / 2;
    if (effectiveSpeed < 100) effectiveSpeed = 100;
    printf("Obstacle ahead (%d cm), slowing down\n", distance);
  }

  // Apply boost if active
  if (speedBoosted) {
    int boostIndex = currentSpeedIndex + 1;
    if (boostIndex >= speedLevelsCount) boostIndex = speedLevelsCount - 1;
    effectiveSpeed = speedLevels[boostIndex];
  }

  // Check for boost timeout
  if (speedBoosted && (millis() - boostStartTime > 2000)) {
    speedBoosted = false;
    printf("Speed reverted after timeout\n");
  }

  // Set the speed
  motors_set_speed(effectiveSpeed);

  // If too close, stop
  if (shouldStop) {
    motors_coast();
    return;
  }

  if (allHigh) {
    allHighCounter++;
    if (allHighCounter >= 3 && !speedBoosted) {
      speedBoosted = true;
      boostStartTime = millis();
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

  // if (left == HIGH && middle == LOW && right == HIGH) {
  //   printf("Forward\n");
  //   motors_forward();
  // } else if (left == LOW && middle == LOW && right == LOW) {
  //   printf("All low\n");
  //   motors_forward();
  // } else if (left == HIGH && middle == LOW && right == LOW) {
  //   printf("Right & middle\n");
  //   motors_forward();
  // } else if (left == LOW && middle == LOW && right == HIGH) {
  //   printf("Left & middle\n");
  //   motors_forward();
  // } else if (left == LOW && middle == HIGH && right == HIGH) {
  //   printf("Left\n");
  //   motors_left();
  // } else if (left == HIGH && middle == HIGH && right == LOW) {
  //   printf("Right\n");
  //   motors_right();
  // }  else if (left == HIGH && middle == HIGH && right == HIGH) {
  //   printf("All high\n");
  //   motors_reverse();
  // } else {
  //   printf("Something wrong\n");
  //   motors_coast();
  // }

  if (middle == LOW) {
    printf("Forward\n");
    lastDirection = DIR_STRAIGHT;
    motors_forward();
  } else if (left == HIGH && right == LOW) {
    printf("Right\n");
    lastDirection = DIR_RIGHT;
    motors_right();
  } else if (left == LOW && right == HIGH) {
    printf("Left\n");
    lastDirection = DIR_LEFT;
    motors_left();
  } else {
    printf("Lost line\n");
    if (lastDirection == DIR_LEFT) {
        motors_left();
    } else if (lastDirection == DIR_RIGHT) {
        motors_right();
    } else if (lastDirection == DIR_STRAIGHT) {
        motors_reverse();
    }else {
        motors_forward();
    }
}
}
