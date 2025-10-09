#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

// Initialize LED pins
void leds_init();

// Update LEDs based on current button states and time
void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right);

#endif
