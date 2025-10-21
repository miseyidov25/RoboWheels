#ifndef MENU_H
#define MENU_H

#include <Arduino.h>

// Initializes the menu and display
void menu_init();
// Updates the menu system and handles user input
void menu_update();
// Gets the current selected menu item index
int menu_get_selected();
// Forces the display to redraw 
void menu_redraw();
// Returns the currently active mode (0=Slave, 1=Autonomous, 2=Control)
int menu_get_mode();

#endif
