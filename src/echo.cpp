#include "echo.h"
#include "pins.h"
#include "motors.h"

// Pin storage
static int _trigPin = -1;
static int _echoPins[4] = {-1, -1, -1, -1}; // 0=front, 1=left, 2=right, 3=back

// Distance storage
static int _distances[4] = {0, 0, 0, 0};

const int MIN_DISTANCE_CM = 1;
const int NUM_READINGS = 3;

void echo_init(int trigPin, int echoPinFront, int echoPinLeft, int echoPinRight, int echoPinBack) {
    _trigPin = trigPin;
    _echoPins[0] = echoPinFront;
    _echoPins[1] = echoPinLeft;
    _echoPins[2] = echoPinRight;
    _echoPins[3] = echoPinBack;

    pinMode(_trigPin, OUTPUT);
    digitalWrite(_trigPin, LOW);

    for (int i = 0; i < 4; i++) {
        pinMode(_echoPins[i], INPUT);
    }
}

// Read a single sensor once
static int echo_readSingle(int sensorIndex) {
    // Temporarily disable all other echo pins
    for (int i = 0; i < 4; i++) {
        if (i != sensorIndex)
            pinMode(_echoPins[i], OUTPUT), digitalWrite(_echoPins[i], LOW);
    }

    // Fire the trigger pulse
    digitalWrite(_trigPin, LOW);
    delayMicroseconds(2);
    digitalWrite(_trigPin, HIGH);
    delayMicroseconds(10);
    digitalWrite(_trigPin, LOW);

    unsigned long duration = pulseIn(_echoPins[sensorIndex], HIGH, 30000UL);

    // Restore echo pins
    for (int i = 0; i < 4; i++) {
        pinMode(_echoPins[i], INPUT);
    }

    if (duration == 0) return 100;

    int distance_cm = (int)(duration * 0.034 / 2);
    if (distance_cm < MIN_DISTANCE_CM) distance_cm = 100;
    return distance_cm;
}

// Get averaged distance
int echo_getDistance(int sensorIndex) {
    if (_trigPin < 0 || sensorIndex < 0 || sensorIndex > 3) return 100;

    int sum = 0;
    for (int i = 0; i < NUM_READINGS; i++) {
        sum += echo_readSingle(sensorIndex);
        delay(5);
    }

    int avgDistance = sum / NUM_READINGS;
    _distances[sensorIndex] = avgDistance;
    return avgDistance;
}

void echo_update() {
    for (int i = 0; i < 4; i++) {
        echo_getDistance(i);
    }

    int front = _distances[0];
    int left  = _distances[1];
    int right = _distances[2];
    int back  = _distances[3];

    const int SAFE_DISTANCE = 20;

    // Obstacle avoidance logic
    if (left < SAFE_DISTANCE && front < SAFE_DISTANCE && right < SAFE_DISTANCE && back < SAFE_DISTANCE) {
        motors_right();
        Serial.println("OBSTACLE SURROUNDING ROBOT - Moving right"); 
    }
    else if (left < SAFE_DISTANCE && front < SAFE_DISTANCE && right < SAFE_DISTANCE) {
        motors_reverse();
        Serial.println("LEFT AND RIGHT AND FRONT OBSTACLE - Turning back");
    }
    else if (left < SAFE_DISTANCE && front < SAFE_DISTANCE) {
        motors_right();
        Serial.println("LEFT AND FRONT OBSTACLE - Turning right");
    }
    else if (left < SAFE_DISTANCE && right < SAFE_DISTANCE) {
        motors_reverse();
        Serial.println("LEFT AND RIGHT OBSTACLE - Turning back");
    }
    else if (front < SAFE_DISTANCE && right < SAFE_DISTANCE) {
        motors_left();
        Serial.println("FRONT AND RIGHT OBSTACLE - Turning left");
    }
    else if (left < SAFE_DISTANCE) {
        motors_right();
        Serial.println("LEFT OBSTACLE - turning right");
    }
    else if (right < SAFE_DISTANCE) {
        motors_left();
        Serial.println("RIGHT OBSTACLE - turning left");
    }
    else if (front < SAFE_DISTANCE) {
        motors_right();
        Serial.println("FRONT OBSTACLE - Turning right");
    }
    else if (back < SAFE_DISTANCE) {
        motors_forward();
        Serial.println("BACK OBSTACLE - Moving forward");
    }
    else if (front > SAFE_DISTANCE) {
        motors_forward();
        Serial.println("Autonomous: Forward");
    }

    // Debug print
    Serial.print("Front: "); Serial.print(front);
    Serial.print(" | Left: "); Serial.print(left);
    Serial.print(" | Right: "); Serial.print(right);
    Serial.print(" | Back: "); Serial.println(back);
}
