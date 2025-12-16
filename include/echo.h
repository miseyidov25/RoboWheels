#ifndef ECHO_H
#define ECHO_H

#include <stdint.h>

// Sensor index mapping
// 0 = front
// 1 = left
// 2 = right
// 3 = back

// Initialize ultrasonic sensors (fixed pins, AVR registers)
void echo_init(void);

// Get distance in centimeters from a specific sensor
int echo_getDistance(uint8_t sensorIndex);

// Update all sensors and perform obstacle-avoidance logic
void echo_update(void);

#endif
