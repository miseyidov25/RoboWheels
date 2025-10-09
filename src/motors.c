#include "motors.h"

// Motor pins
const int IN1 = 7;
const int IN2 = 6;
const int IN3 = 4;
const int IN4 = 2;
const int ENA = 3;  // PWM pin for left motor
const int ENB = 5;  // PWM pin for right motor

// Motor speed (0-255)
int motorSpeed = 185;


void motors_init() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);

    motors_coast();
    analogWrite(ENA, motorSpeed);
    analogWrite(ENB, motorSpeed);
}

void motors_set_speed(int speed) {
    motorSpeed = constrain(speed, 0, 255);
    analogWrite(ENA, motorSpeed);
    analogWrite(ENB, motorSpeed);
}

void motors_forward() {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void motors_reverse() {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void motors_left() {
    digitalWrite(IN1, LOW); digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW);
}

void motors_right() {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, HIGH);
}

void motors_brake() {
    digitalWrite(IN1, HIGH); digitalWrite(IN2, HIGH);
    digitalWrite(IN3, HIGH); digitalWrite(IN4, HIGH);
}

void motors_coast() {
    digitalWrite(IN1, LOW); digitalWrite(IN2, LOW);
    digitalWrite(IN3, LOW); digitalWrite(IN4, LOW);
}

// High-level update called from main
void motors_update(bool fwd) {
    if (fwd) motors_forward();
    else motors_coast();
}
