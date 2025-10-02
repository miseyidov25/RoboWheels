#include "buttons.h"

// Existing buttons
const int BTN_FWD  = 11;
const int BTN_REV  = 12;
const int BTN_RGHT = 13;
const int BTN_LFT  = 10;

// Test button
const int BTN_TEST = 21;

const unsigned long DEBOUNCE_MS = 25;

// Existing button state
bool lastF = HIGH, lastR = HIGH;
bool lastL = HIGH, lastRg = HIGH;
unsigned long tF = 0, tR = 0;
unsigned long tL = 0, tRg = 0;
bool stableF = HIGH, stableR = HIGH;
bool stableL = HIGH, stableRg = HIGH;

// Test button state
static bool forwardActive = false;   // toggle forward mode
static int lastTestState = HIGH;
static unsigned long testDebounceTime = 0;

void buttons_init() {
    pinMode(BTN_FWD, INPUT_PULLUP);
    pinMode(BTN_REV, INPUT_PULLUP);
    pinMode(BTN_RGHT, INPUT_PULLUP);
    pinMode(BTN_LFT, INPUT_PULLUP);

    pinMode(BTN_TEST, INPUT_PULLUP);
    lastTestState = digitalRead(BTN_TEST);
}

void buttons_update() {
    unsigned long now = millis();

    // --- Existing buttons ---
    int rF  = digitalRead(BTN_FWD);
    int rR  = digitalRead(BTN_REV);
    int rL  = digitalRead(BTN_LFT);
    int rRg = digitalRead(BTN_RGHT);

    if (rF  != lastF)  { lastF = rF;  tF  = now; }
    if (rR  != lastR)  { lastR = rR;  tR  = now; }
    if (rL  != lastL)  { lastL = rL;  tL  = now; }
    if (rRg != lastRg) { lastRg = rRg; tRg = now; }

    if (now - tF  > DEBOUNCE_MS) stableF  = lastF;
    if (now - tR  > DEBOUNCE_MS) stableR  = lastR;
    if (now - tL  > DEBOUNCE_MS) stableL  = lastL;
    if (now - tRg > DEBOUNCE_MS) stableRg = lastRg;

    // --- Test button toggle ---
    int reading = digitalRead(BTN_TEST);
    if (reading != lastTestState) {
        testDebounceTime = now;
    }

    if ((now - testDebounceTime) > DEBOUNCE_MS) {
        if (lastTestState == HIGH && reading == LOW) {
            forwardActive = !forwardActive; // toggle forward
        }
    }

    lastTestState = reading;
}

bool is_forward_pressed() { return stableF == LOW; }
bool is_reverse_pressed() { return stableR == LOW; }
bool is_left_pressed()    { return stableL == LOW; }
bool is_right_pressed()   { return stableRg == LOW; }

// Returns true if test button has toggled forward mode
bool is_test_forward_active() { return forwardActive; }
