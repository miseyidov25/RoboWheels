#include "buttons.h"

// --- Configuration ---
const int BTN_TEST = 21;
const unsigned long DEBOUNCE_MS = 100;

// --- State variables ---
static bool forwardActive = false;
static int previousButtonState = HIGH;
static unsigned long previousMillis = 0;

void buttons_init() {
    pinMode(BTN_TEST, INPUT_PULLUP);
    previousButtonState = digitalRead(BTN_TEST);
}

void buttons_update() {
    unsigned long currentMillis = millis();
    int buttonState = digitalRead(BTN_TEST);

    // Debounce logic
    if ((currentMillis - previousMillis) >= DEBOUNCE_MS) {
        previousMillis = currentMillis;

        if (buttonState != previousButtonState) {
            previousButtonState = buttonState;

            if (buttonState == LOW) {
                forwardActive = !forwardActive; // Toggle mode
            }
        }
    }
}

bool is_test_forward_active() {
    return forwardActive;
}
