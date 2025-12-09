#include "echo.h"
#include "pins.h"
#include "motors.h"

// Store pins for each sensor
static int _trigPin = -1;
static int _echoPins[4] = {-1, -1, -1, -1};

// Store last distances for debugging or logic
static int _distances[4] = {0, 0, 0, 0};

void echo_init(int trigPin, int echoPin1, int echoPin2, int echoPin3, int echoPin4) {
    _trigPin = trigPin;
    _echoPins[0] = echoPin1;
    _echoPins[1] = echoPin2;
    _echoPins[2] = echoPin3;
    _echoPins[3] = echoPin4;

    pinMode(_trigPin, OUTPUT);
    digitalWrite(_trigPin, LOW);
    for (int i = 0; i < 4; i++) {
        pinMode(_echoPins[i], INPUT);
    }
}

int echo_getDistance(int sensorIndex) {
    if (_trigPin < 0 || sensorIndex < 0 || sensorIndex > 3) return 0;

    // Trigger pulse
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    // Read echo duration (30ms timeout)
    unsigned long duration = pulseIn(_echoPins[sensorIndex], HIGH, 30000UL);
    if (duration == 0) return 0;

    int distance_cm = (int)(duration * 0.034 / 2);
    _distances[sensorIndex] = distance_cm;
    return distance_cm;
}

void echo_update() {
    // Read all sensors
    for (int i = 0; i < 4; i++) {
        echo_getDistance(i);
    }

    int front = _distances[0];  // middle forward
    int left  = _distances[1];  // left side
    int right = _distances[2];  // right side
    int back  = _distances[3];  // back

    // Simple obstacle avoidance logic
    if (front > 20) {
        motors_forward();
        Serial.println("Autonomous Forward");
    } else {
        motors_coast();
        if (left > right) {
            motors_left();
            Serial.println("Obstacle detected! Turning left");
        } else {
            motors_right();
            Serial.println("Obstacle detected! Turning right");
        }
    }

    // Optional: print distances for debugging
    Serial.print("Front: "); Serial.print(front);
    Serial.print(" | Left: "); Serial.print(left);
    Serial.print(" | Right: "); Serial.print(right);
    Serial.print(" | Back: "); Serial.println(back);
}
