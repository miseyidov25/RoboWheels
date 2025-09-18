#include "motors.h"

// L293D Motor A pins
const int IN1 = 7;
const int IN2 = 8;
const int IN3 = 13;
const int IN4 = 12;
const int ENA = 5;

void motors_init() {
    pinMode(IN1, OUTPUT);
    pinMode(IN2, OUTPUT);
    pinMode(IN3, OUTPUT);
    pinMode(IN4, OUTPUT);
    pinMode(ENA, OUTPUT);
    digitalWrite(ENA, HIGH); // full speed
    motors_coast();
}

void motors_forward() { digitalWrite(IN1, HIGH); digitalWrite(IN2, LOW); }
void motors_reverse() { digitalWrite(IN1, LOW);  digitalWrite(IN2, HIGH); }
void motors_left()    { digitalWrite(IN4, HIGH); digitalWrite(IN3, LOW); }
void motors_right()   { digitalWrite(IN3, HIGH); digitalWrite(IN4, LOW); }
void motors_brake()   { digitalWrite(IN1, HIGH); digitalWrite(IN2, HIGH); }
void motors_coast()   { digitalWrite(IN1, LOW);  digitalWrite(IN2, LOW); }
