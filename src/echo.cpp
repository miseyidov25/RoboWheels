#include "echo.h"

// Internal pin storage
static int _trigPin = -1;
static int _echoPin = -1;

void echo_init(int trigPin, int echoPin) {
    _trigPin = trigPin;
    _echoPin = echoPin;

    pinMode(_trigPin, OUTPUT);
    digitalWrite(_trigPin, LOW);
    pinMode(_echoPin, INPUT);
}

int echo_getDistance() {
    if (_trigPin < 0 || _echoPin < 0) return 0; // not initialized

    // Trigger the sensor (10us pulse)
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Measure echo duration (timeout 30000 us = 30 ms)
    unsigned long duration = pulseIn(_echoPin, HIGH, 30000UL);

    if (duration == 0) return 0; // no echo within timeout

    int distance_cm = (int)(duration * 0.034 / 2);
    return distance_cm;
}
