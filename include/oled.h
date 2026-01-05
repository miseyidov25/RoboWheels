#ifndef OLED_H
#define OLED_H

#include <stdbool.h>

// UI state: true = menu visible, false = status screen visible
extern bool menuActive;

void oled_init();
void oled_update();
Mode ui_get_current_mode();

#endif
