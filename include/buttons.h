#ifndef BUTTONS_H
#define BUTTONS_H

#include <Arduino.h>

void buttons_init();
void buttons_update();
bool is_forward_pressed();
bool is_reverse_pressed();
bool is_left_pressed();
bool is_right_pressed();

#endif
