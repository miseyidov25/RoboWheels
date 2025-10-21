#ifndef LEDS_H
#define LEDS_H

#include <Arduino.h>

// Initialize LED pins
void leds_init();

// Update directional LEDs (turn signals, brake lights)
void leds_update(unsigned long now, bool fwd, bool rev, bool left, bool right);

// Update RGB LED(ON/OFF, mode)
// mode: 0 = Slave, 1 = Autonomous, 2 = Control
void ledsRGB_update(bool robot_on, int mode);

#endif
