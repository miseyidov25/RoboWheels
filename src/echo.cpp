#include "echo.h"
#include "motors.h"
#include <avr/io.h>
#include <util/delay.h>
#include "pins.h"

// Sensor index mapping
// 0 = front (D7 / PD7)
// 1 = left  (D11 / PB3)
// 2 = right (D12 / PB4)
// 3 = back  (D13 / PB5)

#define NUM_SENSORS       4
#define NUM_READINGS      3
#define MIN_DISTANCE_CM   2
#define MAX_DISTANCE_CM   100
#define SAFE_DISTANCE     30

#define ECHO_TIMEOUT_US   30000UL   // 30ms

// Cached distances
static int distances[NUM_SENSORS] = {0};

// --------------------------------------------------
// Trigger pulse
// --------------------------------------------------
static inline void trig_pulse(void) {
    PORTB &= ~(1 << PB0);
    _delay_us(2);
    PORTB |=  (1 << PB0);
    _delay_us(10);
    PORTB &= ~(1 << PB0);
}

// --------------------------------------------------
// Measure echo pulse width
// --------------------------------------------------
static unsigned long read_echo_us(volatile uint8_t* pinReg, uint8_t bit) {
    unsigned long time = 0;

    // Wait for HIGH
    while (!(*pinReg & (1 << bit))) {
        if (++time >= ECHO_TIMEOUT_US) return 0;
        _delay_us(1);
    }

    // Measure HIGH width
    time = 0;
    while (*pinReg & (1 << bit)) {
        if (++time >= ECHO_TIMEOUT_US) break;
        _delay_us(1);
    }

    return time;
}

// --------------------------------------------------
// Single sensor read
// --------------------------------------------------
static int echo_readSingle(uint8_t sensorIndex) {
    unsigned long duration_us = 0;

    trig_pulse();
    _delay_us(200); // sensor settle time

    switch (sensorIndex) {
        case 0: duration_us = read_echo_us(&PIND, PD7); break;
        case 1: duration_us = read_echo_us(&PINB, PB3); break;
        case 2: duration_us = read_echo_us(&PINB, PB4); break;
        case 3: duration_us = read_echo_us(&PINB, PB5); break;
        default: return MAX_DISTANCE_CM;
    }

    if (duration_us == 0) return MAX_DISTANCE_CM;

    int distance_cm = (int)(duration_us * 0.034f / 2.0f);

    if (distance_cm < MIN_DISTANCE_CM || distance_cm > MAX_DISTANCE_CM)
        return MAX_DISTANCE_CM;

    return distance_cm;
}

// --------------------------------------------------
// Initialization
// --------------------------------------------------

void echo_init(void) {
    // TRIG = D8 (PB0)
    DDRB  |=  (1 << PB0);
    PORTB &= ~(1 << PB0);

    // Echo pins INPUT
    DDRD &= ~(1 << PD7);
    DDRB &= ~((1 << PB3) | (1 << PB4) | (1 << PB5));
}

int echo_getDistance(uint8_t sensorIndex) {
    if (sensorIndex >= NUM_SENSORS) return MAX_DISTANCE_CM;

    int sum = 0;
    for (uint8_t i = 0; i < NUM_READINGS; i++) {
        sum += echo_readSingle(sensorIndex);
        _delay_ms(5);
    }

    distances[sensorIndex] = sum / NUM_READINGS;
    return distances[sensorIndex];
}

// --------------------------------------------------
// Main obstacle logic
// --------------------------------------------------
void echo_update(void) {
    for (uint8_t i = 0; i < NUM_SENSORS; i++) {
        echo_getDistance(i);
    }

    int front = distances[0];
    int left  = distances[1];
    int right = distances[2];
    int back  = distances[3];

    if (left < SAFE_DISTANCE && front < SAFE_DISTANCE &&
        right < SAFE_DISTANCE && back < SAFE_DISTANCE) {

        motors_right();
        Serial.println("SURROUNDED - turning right");
    }
    else if (front < SAFE_DISTANCE && left < SAFE_DISTANCE) {
        motors_right();
        Serial.println("FRONT+LEFT - turning right");
    }
    else if (front < SAFE_DISTANCE && right < SAFE_DISTANCE) {
        motors_left();
        Serial.println("FRONT+RIGHT - turning left");
    }
    else if (left < SAFE_DISTANCE) {
        motors_right();
        Serial.println("LEFT obstacle");
    }
    else if (right < SAFE_DISTANCE) {
        motors_left();
        Serial.println("RIGHT obstacle");
    }
    else if (front < SAFE_DISTANCE) {
        motors_right();
        Serial.println("FRONT obstacle");
    }
    else if (back < SAFE_DISTANCE) {
        motors_forward();
        Serial.println("BACK obstacle");
    }
    else {
        motors_forward();
        Serial.println("Forward");
    }

    Serial.print("F: "); Serial.print(front);
    Serial.print(" L: "); Serial.print(left);
    Serial.print(" R: "); Serial.print(right);
    Serial.print(" B: "); Serial.println(back);
}
