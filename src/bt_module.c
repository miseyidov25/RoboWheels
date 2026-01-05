#include <avr/io.h>
#include <stdlib.h>
#include <stdbool.h>
#include "bt.h"
#include "motors.h"

// UART functions
void uart_init() {
    UBRR0H = 0;
    UBRR0L = 103; // 9600 baud at 16MHz
    UCSR0B = (1 << RXEN0) | (1 << TXEN0);
    UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void uart_putc(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

void uart_puts(const char* str) {
    while (*str) uart_putc(*str++);
}

void uart_print(const char* str) {
    uart_puts(str);
}

void uart_println(const char* str) {
    uart_puts(str);
    uart_puts("\r\n");
}

void uart_print_int(int num) {
    char buf[12];
    itoa(num, buf, 10);
    uart_print(buf);
}

void uart_print_char(char c) {
    while (!(UCSR0A & (1 << UDRE0)));
    UDR0 = c;
}

int uart_available() {
    return (UCSR0A & (1 << RXC0)) ? 1 : 0;
}

char uart_read() {
    return UDR0;
}

// External globals
extern Mode currentMode;
extern bool menuActive;
extern bool hasSelectedMode;
extern int currentSpeedIndex;
extern int speedLevels[];
extern const int speedLevelsCount;

// Millis function
extern unsigned long millis();

// Track active command and previous state
static char activeCmd = '\0';
static char prevCmd = '\0';
static unsigned long lastCmdTime = 0;  // millis() of last command
const unsigned long STOP_DELAY = 500;  // 500 ms stop delay

void bt_init() {
    // Serial already initialized in main.cpp
}

void executeCommand(char cmd) {
    // Lowercase mode letters go to menu
    if (cmd >= 'a' && cmd <= 'z') {
        if (cmd == 'w' || cmd == 'x' || cmd == 'u') {
            currentMode = NONE; // return to menu
            menuActive = true;
            hasSelectedMode = false;
            motors_coast();
            uart_println("Returning to menu...");
            return;
        } else {
            cmd -= 32;  // Normalize other lowercase letters
        }
    }

    // Speed commands
    if (cmd >= '1' && cmd <= '4') {
        currentSpeedIndex = cmd - '1';
        if (currentSpeedIndex < 0) currentSpeedIndex = 0;
        if (currentSpeedIndex >= speedLevelsCount) currentSpeedIndex = speedLevelsCount - 1;
        motors_set_speed(speedLevels[currentSpeedIndex]);
        uart_print("Speed: ");
        uart_print_int(speedLevels[currentSpeedIndex]);
        uart_println("");
        return;
    }

    // Mode commands
    switch (cmd) {
        case 'W': currentMode = AUTONOMOUS; menuActive = false; hasSelectedMode = true; uart_println("Mode: AUTONOMOUS"); return;
        case 'X': currentMode = SLAVE;      menuActive = false; hasSelectedMode = true; uart_println("Mode: SLAVE");      return;
        case 'U': currentMode = MANUAL;     menuActive = false; hasSelectedMode = true; uart_println("Mode: MANUAL");     return;
    }

    // Movement commands only in MANUAL
    if (currentMode != MANUAL) {
        uart_print("Ignored command '"); uart_print_char(cmd); uart_println("' - not in MANUAL mode");
        return;
    }

    // Update active command
    activeCmd = cmd;
    lastCmdTime = millis(); // reset timer on every command
}

void bt_update() {
    bool gotCmd = false;

    // Read all serial input
    while (uart_available()) {
        char cmd = uart_read();
        if (cmd == '\r' || cmd == '\n' || cmd == ' ' || cmd == '\0') continue;
        executeCommand(cmd);
        gotCmd = true;
    }

    unsigned long now = millis();

    if (activeCmd != prevCmd && activeCmd != '\0') {
        switch (activeCmd) {
            case 'F': motors_forward();      uart_println("Got: F (Forward)"); break;
            case 'B': motors_reverse();      uart_println("Got: B (Reverse)"); break;
            case 'L': motors_left();         uart_println("Got: L (Left)"); break;
            case 'R': motors_right();        uart_println("Got: R (Right)"); break;
            case 'H': motors_correctright(); uart_println("Got: H (Correct Right)"); break;
            case 'G': motors_correctleft();  uart_println("Got: G (Correct Left)"); break;
            case 'S':
            case 'C': motors_coast();        uart_println("Got: S/C (Stop/Coast)"); break;
            default:  uart_print("Unknown command: "); uart_print_char(activeCmd); uart_println(""); break;
        }
        prevCmd = activeCmd;
    }

    // Send stop only if no command received for STOP_DELAY
    if (activeCmd != '\0' && (now - lastCmdTime > STOP_DELAY)) {
        motors_coast();
        uart_println("Got: S/C (Stop/Coast)");
        activeCmd = '\0';
        prevCmd = '\0';
    }

    // If no serial input, don't overwrite activeCmd until STOP_DELAY
    if (!gotCmd && activeCmd == '\0') {
        prevCmd = '\0';
    }
}

char bt_get_active_cmd() {
    return activeCmd;
}