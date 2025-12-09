#ifndef ECHO_H
#define ECHO_H

#include <Arduino.h>

// Initialize the 3 front sensors
// trigPin: trigger pin for all sensors
// echoPinFront: front sensor
// echoPinLeft: left sensor
// echoPinRight: right sensor
void echo_init(int trigPin, int echoPinFront, int echoPinLeft, int echoPinRight);

// Get distance from a specific sensor (0=front,1=left,2=right)
int echo_getDistance(int sensorIndex);

// Update autonomous behavior based on sensor readings
void echo_update();

#endif
