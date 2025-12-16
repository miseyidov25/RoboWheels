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

// Distances
static int _distances[4] = {0, 0, 0, 0};

const int MIN_DISTANCE_CM = 1;
const int NUM_READINGS   = 3;


static inline void trig_pulse() {
    PORTB &= ~(1 << PB0);   // D8 LOW
    _delay_us(2);
    PORTB |=  (1 << PB0);   // D8 HIGH
    _delay_us(10);
    PORTB &= ~(1 << PB0);   // D8 LOW
}

static unsigned long read_echo(volatile uint8_t* pinReg, uint8_t bit) {
    unsigned long timeout = 0;

    // wait for HIGH
    while (!(*pinReg & (1 << bit))) {
        if (++timeout > 30000) return 0;
    }

    unsigned long width = 0;
    while (*pinReg & (1 << bit)) {
        if (++width > 30000) break;
    }

    return width;
}

static int echo_readSingle(int sensorIndex) {
    trig_pulse();

    unsigned long duration = 0;

    switch (sensorIndex) {
        case 0: duration = read_echo(&PIND, PD7); break; // front
        case 1: duration = read_echo(&PINB, PB3); break; // left
        case 2: duration = read_echo(&PINB, PB4); break; // right
        case 3: duration = read_echo(&PINB, PB5); break; // back
        default: return 100;
    }

    if (duration == 0) return 100;

    int distance_cm = (int)(duration * 0.034 / 2);
    if (distance_cm < MIN_DISTANCE_CM) distance_cm = 100;

    return distance_cm;
}

// PUBLIC API 

void echo_init(int, int, int, int, int) {
    // TRIG = D8 = PB0
    DDRB  |=  (1 << PB0);
    PORTB &= ~(1 << PB0);

    // Echo pins as INPUT
    DDRD &= ~(1 << PD7);                     // D7
    DDRB &= ~((1 << PB3) | (1 << PB4) | (1 << PB5)); // D11,12,13
}

int echo_getDistance(int sensorIndex) {
    if (sensorIndex < 0 || sensorIndex > 3) return 100;

    int sum = 0;
    for (int i = 0; i < NUM_READINGS; i++) {
        sum += echo_readSingle(sensorIndex);
        _delay_ms(5);
    }

    _distances[sensorIndex] = sum / NUM_READINGS;
    return _distances[sensorIndex];
}

void echo_update() {
    for (int i = 0; i < 4; i++) {
        echo_getDistance(i);
    }

    int front = _distances[0];
    int left  = _distances[1];
    int right = _distances[2];
    int back  = _distances[3];

    const int SAFE_DISTANCE = 30;

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