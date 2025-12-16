#ifndef LEDS_H
#define LEDS_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

// Initialize LED pins
void leds_init(void);

// Update LEDs based on current button states and time
void leds_update(void);

#ifdef __cplusplus
}
#endif

#endif
