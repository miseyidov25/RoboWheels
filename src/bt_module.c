/* src/bt_module_c.c
   AVR C equivalent of the Arduino sketch in bt_module.c.
   Uses hardware UART0 (pins 0/1 on an Uno) at 9600 baud to talk to HC-06.
   Adjust F_CPU if your MCU clock differs (this assumes 16 MHz).
*/

#include <avr/io.h>
#include <util/delay.h>
#include <stdint.h>

/* Pin mapping taken from include/bt.h in your project:
   IN1 = 7, IN2 = 8, IN3 = 13, IN4 = 12, ENA = 5
   We'll map Arduino pin numbers to AVR ports for an UNO (ATmega328P).
   Arduino Digital -> Port/Bit:
     0  PD0
     1  PD1
     2  PD2
     3  PD3
     4  PD4
     5  PC5  (analog pin A5)
     6  PD6
     7  PD7
     8  PB0
     9  PB1
    10  PB2
    11  PB3
    12  PB4
    13  PB5
*/
#define F_CPU 16000000UL
#define BAUD 9600
#define UBRR_VALUE ((F_CPU/16/BAUD)-1)

/* Arduino pin -> AVR port/bit helpers */
#define PIN_TO_PORTB(pin) ((pin)==8 || (pin)==9 || (pin)==10 || (pin)==11 || (pin)==12 || (pin)==13)
#define PIN_TO_PORTC(pin) ((pin)==5) /* only ENA=5 maps to PC5 */
#define PIN_TO_PORTD(pin) ((pin)==0 || (pin)==1 || (pin)==2 || (pin)==3 || (pin)==4 || (pin)==6 || (pin)==7)

/* Specific bit macros for our pins */
#define IN1_PIN 7
#define IN2_PIN 8
#define IN3_PIN 13
#define IN4_PIN 12
#define ENA_PIN 5

/* Map to AVR DDR/PORT bits */
static inline void pinMode_out(uint8_t arduino_pin) {
    if (PIN_TO_PORTD(arduino_pin)) {
        /* map to DDRD */
        uint8_t bit = (arduino_pin == 7) ? PD7 :
                      (arduino_pin == 6) ? PD6 :
                      (arduino_pin == 4) ? PD4 :
                      (arduino_pin == 3) ? PD3 :
                      (arduino_pin == 2) ? PD2 :
                      (arduino_pin == 1) ? PD1 :
                      PD0; /* fallback */
        DDRD |= (1 << bit);
    } else if (PIN_TO_PORTB(arduino_pin)) {
        uint8_t bit = (arduino_pin == 8) ? PB0 :
                      (arduino_pin == 9) ? PB1 :
                      (arduino_pin == 10)? PB2 :
                      (arduino_pin == 11)? PB3 :
                      (arduino_pin == 12)? PB4 :
                      PB5; /* 13 */
        DDRB |= (1 << bit);
    } else if (PIN_TO_PORTC(arduino_pin)) {
        uint8_t bit = (arduino_pin == 5) ? PC5 : PC0;
        DDRC |= (1 << bit);
    }
}

static inline void digitalWrite_high(uint8_t arduino_pin) {
    if (PIN_TO_PORTD(arduino_pin)) {
        uint8_t bit = (arduino_pin == 7) ? PD7 :
                      (arduino_pin == 6) ? PD6 :
                      (arduino_pin == 4) ? PD4 :
                      (arduino_pin == 3) ? PD3 :
                      (arduino_pin == 2) ? PD2 :
                      (arduino_pin == 1) ? PD1 :
                      PD0;
        PORTD |= (1 << bit);
    } else if (PIN_TO_PORTB(arduino_pin)) {
        uint8_t bit = (arduino_pin == 8) ? PB0 :
                      (arduino_pin == 9) ? PB1 :
                      (arduino_pin == 10)? PB2 :
                      (arduino_pin == 11)? PB3 :
                      (arduino_pin == 12)? PB4 :
                      PB5;
        PORTB |= (1 << bit);
    } else if (PIN_TO_PORTC(arduino_pin)) {
        uint8_t bit = (arduino_pin == 5) ? PC5 : PC0;
        PORTC |= (1 << bit);
    }
}

static inline void digitalWrite_low(uint8_t arduino_pin) {
    if (PIN_TO_PORTD(arduino_pin)) {
        uint8_t bit = (arduino_pin == 7) ? PD7 :
                      (arduino_pin == 6) ? PD6 :
                      (arduino_pin == 4) ? PD4 :
                      (arduino_pin == 3) ? PD3 :
                      (arduino_pin == 2) ? PD2 :
                      (arduino_pin == 1) ? PD1 :
                      PD0;
        PORTD &= ~(1 << bit);
    } else if (PIN_TO_PORTB(arduino_pin)) {
        uint8_t bit = (arduino_pin == 8) ? PB0 :
                      (arduino_pin == 9) ? PB1 :
                      (arduino_pin == 10)? PB2 :
                      (arduino_pin == 11)? PB3 :
                      (arduino_pin == 12)? PB4 :
                      PB5;
        PORTB &= ~(1 << bit);
    } else if (PIN_TO_PORTC(arduino_pin)) {
        uint8_t bit = (arduino_pin == 5) ? PC5 : PC0;
        PORTC &= ~(1 << bit);
    }
}

/* PWM for ENA on Arduino pin 5 -> PC5 (which is ADC5) on Uno; BUT PWM pins on Uno:
   3 (PD3), 5 (PD5), 6 (PD6), 9 (PB1), 10 (PB2), 11 (PB3).
   Arduino pin 5 is actually PD5 on Uno, not PC5. (Important correction).
   Update mapping: ENA=5 -> PD5 (OC0B)
*/

#undef PIN_TO_PORTC
#define PIN_TO_PORTC(pin) (0) /* no PC pins used now */

/* fix mapping helpers for our chosen pins */
static inline void pinMode_out_fixed(uint8_t arduino_pin) {
    if (arduino_pin == 5) { /* PD5 */
        DDRD |= (1 << PD5);
    } else if (arduino_pin == 7) { /* PD7 */
        DDRD |= (1 << PD7);
    } else if (arduino_pin == 8) { /* PB0 */
        DDRB |= (1 << PB0);
    } else if (arduino_pin == 12) { /* PB4 */
        DDRB |= (1 << PB4);
    } else if (arduino_pin == 13) { /* PB5 */
        DDRB |= (1 << PB5);
    }
}

static inline void digitalWrite_high_fixed(uint8_t arduino_pin) {
    if (arduino_pin == 5) PORTD |= (1 << PD5);
    else if (arduino_pin == 7) PORTD |= (1 << PD7);
    else if (arduino_pin == 8) PORTB |= (1 << PB0);
    else if (arduino_pin == 12) PORTB |= (1 << PB4);
    else if (arduino_pin == 13) PORTB |= (1 << PB5);
}

static inline void digitalWrite_low_fixed(uint8_t arduino_pin) {
    if (arduino_pin == 5) PORTD &= ~(1 << PD5);
    else if (arduino_pin == 7) PORTD &= ~(1 << PD7);
    else if (arduino_pin == 8) PORTB &= ~(1 << PB0);
    else if (arduino_pin == 12) PORTB &= ~(1 << PB4);
    else if (arduino_pin == 13) PORTB &= ~(1 << PB5);
}

/* Motor functions using fixed helpers */
void motors_forward(void) {
    digitalWrite_high_fixed(IN1_PIN);
    digitalWrite_low_fixed(IN2_PIN);
    digitalWrite_high_fixed(IN3_PIN);
    digitalWrite_low_fixed(IN4_PIN);
}
void motors_reverse(void) {
    digitalWrite_low_fixed(IN1_PIN);
    digitalWrite_high_fixed(IN2_PIN);
    digitalWrite_low_fixed(IN3_PIN);
    digitalWrite_high_fixed(IN4_PIN);
}
void motors_left(void) {
    digitalWrite_low_fixed(IN1_PIN);
    digitalWrite_high_fixed(IN2_PIN);
    digitalWrite_high_fixed(IN3_PIN);
    digitalWrite_low_fixed(IN4_PIN);
}
void motors_right(void) {
    digitalWrite_high_fixed(IN1_PIN);
    digitalWrite_low_fixed(IN2_PIN);
    digitalWrite_low_fixed(IN3_PIN);
    digitalWrite_high_fixed(IN4_PIN);
}
void motors_brake(void) {
    digitalWrite_high_fixed(IN1_PIN);
    digitalWrite_high_fixed(IN2_PIN);
    digitalWrite_high_fixed(IN3_PIN);
    digitalWrite_high_fixed(IN4_PIN);
}
void motors_coast(void) {
    digitalWrite_low_fixed(IN1_PIN);
    digitalWrite_low_fixed(IN2_PIN);
    digitalWrite_low_fixed(IN3_PIN);
    digitalWrite_low_fixed(IN4_PIN);
}

/* UART basic functions for 9600 baud on UBRR_VALUE */
static void uart_init(void) {
    /* Set baud rate */
    UBRR0H = (uint8_t)(UBRR_VALUE >> 8);
    UBRR0L = (uint8_t)UBRR_VALUE;
    /* Enable receiver and transmitter */
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    /* Set frame format: 8data, 1stop bit */
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

/* Return non-zero if data is available */
static inline uint8_t uart_available(void) {
    return (UCSR0A & (1 << RXC0)) != 0;
}

/* Read a byte (blocking if no data) */
static uint8_t uart_read(void) {
    while (!(UCSR0A & (1 << RXC0)));
    return UDR0;
}

/* Write a byte (blocking until ready) */
static void uart_write(uint8_t b) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = b;
}

/* simple print char sequence to UART (no printf) */
static void uart_print(const char *s) {
    while (*s) {
        uart_write((uint8_t)*s++);
    }
}

/* Setup equivalent */
static void setup_hw(void) {
    /* Initialize motor pins as outputs */
    pinMode_out_fixed(IN1_PIN);
    pinMode_out_fixed(IN2_PIN);
    pinMode_out_fixed(IN3_PIN);
    pinMode_out_fixed(IN4_PIN);
    pinMode_out_fixed(ENA_PIN);

    /* Configure PWM for ENA (Arduino pin 5 -> PD5 -> OC0B)
       We'll set Timer0 Phase Correct PWM on OC0B and set OCR0B = 255 for full speed.
    */
    /* Fast PWM, non-inverting on OC0B (using Timer0)
       WGM02:0 = 3 (fast PWM) requires setting WGM01 and WGM00 (Timer0)
       For simplicity, use Phase Correct (WGM00 = 1, WGM01 = 0) so OC0B works; but
       many Arduino mappings use different timers. Keep it simple: set OC0B as output and write PORTD high for full speed
       because configuring timers in AVR C is project-specific. */

    /* Mark ENA high (full speed) */
    digitalWrite_high_fixed(ENA_PIN);

    /* Start coasting */
    motors_coast();
}

/* Main loop */
int main(void) {
    uart_init();
    setup_hw();
    uart_print("Bluetooth Robot Ready\r\n");

    for (;;) {
        /* Forward anything typed on USB serial is not present in plain AVR; we only listen to UART0 from HC-06 */
        if (uart_available()) {
            char cmd = (char)uart_read();

            /* Optionally echo */
            uart_write('G'); uart_write(':'); uart_write(' '); uart_write((uint8_t)cmd); uart_write('\r'); uart_write('\n');

            switch (cmd) {
                case 'F': motors_forward();  break;
                case 'B': motors_reverse();  break;
                case 'L': motors_left();     break;
                case 'R': motors_right();    break;
                case 'S': motors_brake();    break;
                case 'C': motors_coast();    break;
                default: /* ignore */       break;
            }
        }
    }
    return 0;
}