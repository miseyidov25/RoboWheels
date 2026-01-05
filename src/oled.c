#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "bt.h"
#include "oled.h"
#include "motors.h"
#include "pins.h"
#include "line.h"

// PIN CONFIG

#define BTN_NEXT_PIN PC2
#define BTN_SELECT_PIN PC3

// UI STATE 

static Mode menuSelection = MANUAL;
bool menuActive = false;
bool hasSelectedMode = false;

// Last sampled button states
static bool lastNextState   = 1;
static bool lastSelectState = 1;

// FORWARD DECLARATIONS 

static const char* modeToString(Mode m);

void oled_init();
void oled_update();
Mode ui_get_current_mode();

// HELPER FUNCTIONS 

static const char* getCurrentDirection() {
    return motorDirection == 1 ? "Fwd" :
           motorDirection == -1 ? "Rev" :
           motorDirection == 2 ? "Left" :
           motorDirection == 3 ? "Right" : "Stop";
}

static int getCurrentSpeed() {
    return currentEffectiveSpeed();
}

static const char* modeToString(Mode m) {
    switch (m) {
        case AUTONOMOUS: return "Auto";
        case SLAVE:      return "Slave";
        case MANUAL:     return "Manual";
        default:         return "?";
    }
}

// DRAWING FUNCTIONS (UART PRINTS)

void drawStatusScreen() {
    char buf[40];
    sprintf(buf, "Mode: %s\r\n", modeToString(currentMode));
    uart_puts(buf);
    sprintf(buf, "Dir : %s\r\n", getCurrentDirection());
    uart_puts(buf);
    sprintf(buf, "Spd : %d\r\n", getCurrentSpeed());
    uart_puts(buf);
}

void drawMenuScreen() {
    uart_puts("Select mode:\r\n");
    for (int i = MANUAL; i <= SLAVE; ++i) {
        if (menuSelection == (Mode)i)
            uart_puts("> ");
        else
            uart_puts("  ");
        uart_puts(modeToString((Mode)i));
        uart_puts("\r\n");
    }
}

void drawSplashScreen() {
    uart_puts("RoboWheel\r\nMode Selection First\r\n");
}

// BUTTON HANDLING 

static void handleNextPressed() {
    if (!menuActive) return;
    int idx = (int)menuSelection;
    idx++;
    if (idx > SLAVE) idx = MANUAL;  // wrap around
    menuSelection = (Mode)idx;
}

static void handleSelectPressed() {
    if (menuActive) {
        currentMode = menuSelection;
        hasSelectedMode = true;
        menuActive = false;
        uart_puts("Mode selected: ");
        uart_puts(modeToString(currentMode));
        uart_puts("\r\n");
    } else {
        menuActive = true;
        menuSelection = currentMode;
        uart_puts("Menu reopened\r\n");
    }
}

void readButtons() {
    bool currentNext = (PINC & (1 << BTN_NEXT_PIN)) == 0;
    bool currentSelect = (PINC & (1 << BTN_SELECT_PIN)) == 0;

    // NEXT button
    if (menuActive && lastNextState == 1 && currentNext == 0) {
        handleNextPressed();
    }

    // SELECT button
    if (lastSelectState == 1 && currentSelect == 0) {
        handleSelectPressed();
    }

    lastNextState = currentNext;
    lastSelectState = currentSelect;
}

// UI PUBLIC API

void oled_init() {
    // Set buttons as input with pullup
    DDRC &= ~((1 << BTN_NEXT_PIN) | (1 << BTN_SELECT_PIN));
    PORTC |= ((1 << BTN_NEXT_PIN) | (1 << BTN_SELECT_PIN));

    lastNextState = (PINC & (1 << BTN_NEXT_PIN)) != 0;
    lastSelectState = (PINC & (1 << BTN_SELECT_PIN)) != 0;

    drawSplashScreen();
    _delay_ms(1500);

    menuActive = true;
    hasSelectedMode = false;
    currentMode = MANUAL;
    menuSelection = currentMode;
}

void oled_update() {
    readButtons();

    if (menuActive)
        drawMenuScreen();
    else
        drawStatusScreen();

    _delay_ms(20);
}

Mode ui_get_current_mode() {
    return currentMode;
}
