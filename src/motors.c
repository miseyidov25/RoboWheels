#include "motors.h"

// Motor pins
const int IN1 = 6;
const int IN2 = 7;
const int IN3 = 2;
const int IN4 = 4;
const int ENA = 3;
const int ENB = 5;

void motors_init() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    pinMode(ENB, OUTPUT);
    digitalWrite(ENA, HIGH);
    digitalWrite(ENB, HIGH);
    motors_coast();
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
void motors_update(bool fwd, bool rev, bool left, bool right, bool obstacle) {
    if (obstacle) {
        motors_brake();
        return;
    }

    if (fwd && !rev) motors_forward();
    else if (rev && !fwd) motors_reverse();
    else if (fwd && rev) motors_brake();
    else motors_coast();

    if (left && !right) motors_left();
    else if (right && !left) motors_right();
    else if (left && right) motors_brake();
}
