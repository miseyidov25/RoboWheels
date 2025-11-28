#ifndef ECHO_H
#define ECHO_H

#include <Arduino.h>

// Initialize the sensor pins
void echo_init(int trigPin, int echoPin);

// Read distance in cm
int echo_getDistance();

void echo_update(); 

#endif
