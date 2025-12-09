#ifndef ECHO_H
#define ECHO_H

#include <Arduino.h>

// Initialize the 4 sensor pins
void echo_init(int trigPin, int echoPin1, int echoPin2, int echoPin3, int echoPin4);

// Read distances (in cm) from all 4 sensors
int echo_getDistance(int sensorIndex);  // sensorIndex 0-3

// Update autonomous behavior based on distances
void echo_update();

#endif
