#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

// Initialize LED pins
void leds_init();

// Update LEDs based on current button states, time and mode (0=Slave,1=Autonomous,2=Control)
void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right, int mode);

#endif
