#ifndef ECHO_H
#define ECHO_H

#include <Arduino.h>

// Initialize the sensor pins
void echo_init(int trigPin, int echoPin, int echoPin2, int echoPin3, int echoPin4);

// Read distance in cm
void echo_update();

#endif
