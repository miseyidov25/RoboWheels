#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdbool.h>
#include "pins.h"
#include "motors.h"
#include "line.h"
#include "bt.h"
#include "echo.h"
#include "oled.h"
#include "leds.h"

// GLOBAL MODE VARIABLE DEFINED HERE 
Mode currentMode = NONE;

// Speed Control
int motorSpeed = 230;
int motorSpeedAuto = 110;

unsigned long previousMillis = 0;

// Millis implementation using Timer0
volatile unsigned long millis_counter = 0;

ISR(TIMER0_OVF_vect) {
    millis_counter++;
}

void millis_init() {
    TCCR0A = 0;
    TCCR0B = (1 << CS01) | (1 << CS00); // prescaler 64
    TIMSK0 |= (1 << TOIE0);
    sei();
}

unsigned long millis() {
    return millis_counter;
}

// TWI initialization for Wire
void twi_init() {
    TWBR = 72; // 100kHz at 16MHz
    TWCR = (1 << TWEN);
}

// Pin mode definitions
#define INPUT 0
#define OUTPUT 1

void pinMode(uint8_t pin, uint8_t mode) {
    volatile uint8_t *ddr;
    uint8_t bit;
    if (pin >= 0 && pin <= 7) {
        ddr = &DDRD;
        bit = pin;
    } else if (pin >= 8 && pin <= 13) {
        ddr = &DDRB;
        bit = pin - 8;
    } else if (pin >= 14 && pin <= 19) { // A0-A5
        ddr = &DDRC;
        bit = pin - 14;
    } else {
        return;
    }
    if (mode == OUTPUT) {
        *ddr |= (1 << bit);
    } else {
        *ddr &= ~(1 << bit);
    }
}

// make it available to other files:
int currentEffectiveSpeed() {
    return (currentMode == AUTONOMOUS) ? motorSpeedAuto : motorSpeed;
}

void setup() {
    uart_init(); // Serial.begin(9600)

    twi_init(); // Wire.begin()

    motors_init();
    echo_init();

    pinMode(LEFT_SENSOR, INPUT);
    pinMode(MIDDLE_SENSOR, INPUT);
    pinMode(RIGHT_SENSOR, INPUT);

    oled_init(); 
    bt_init();
    leds_init();

    millis_init();
}

void loop() {
    oled_update();  
    bt_update();   
    
    char cmd = bt_get_active_cmd();
    leds_update(millis(), cmd == 'F', cmd == 'B', cmd == 'L' || cmd == 'G', cmd == 'R' || cmd == 'H');

    if (!menuActive) {
        // Autonomous mode
        if (currentMode == AUTONOMOUS) {
            echo_update();
        }
        // Slave mode (line follow)
        else if (currentMode == SLAVE) {
            line_update();
        }
        // Manual mode -> nothing here (Bluetooth handles it)
    }
}

int main(void) {
    setup();
    while (1) {
        loop();
    }
    return 0;
}