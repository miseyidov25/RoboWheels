#include "echo.h"

// Sensor pins
static int _trigPin;
static int _echoPin;

void echo_init(int trigPin, int echoPin) {
    _trigPin = trigPin;
    _echoPin = echoPin;

    pinMode(_trigPin, OUTPUT);
    digitalWrite(_trigPin, LOW);
    pinMode(_echoPin, INPUT);
}

int echo_getDistance() {
    // Trigger the sensor
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Measure echo duration
    unsigned long duration = pulseIn(_echoPin, HIGH, 30000); // 30ms timeout

    if (duration == 0) return 0; // nothing detected

    int distance_cm = duration * 0.034 / 2;
    return distance_cm;
}
