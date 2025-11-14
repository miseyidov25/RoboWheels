#ifndef UI_H
#define UI_H

#include <Arduino.h>

// ===================== ENUMS & STATE =====================

// This Enum defines the different robot operation modes.
// It is placed in the header file so other modules can access it.
enum Mode {
  MODE_AUTONOMOUS = 0,
  MODE_SLAVE      = 1,
  MODE_REMOTE     = 2,
  MODE_COUNT      = 3
};


// ===================== PUBLIC FUNCTIONS =====================

/**
 * @brief Initializes the UI module (buttons, display, and EEPROM).
 * This function should be called once in the main setup().
 */
void ui_init();

/**
 * @brief Updates the UI module (checks buttons, updates runtime, draws to display).
 * This function should be called continuously in the main loop().
 */
void ui_update();

/**
 * @brief Returns the currently active mode.
 * @return Mode The currently selected mode (AUTONOMOUS, SLAVE, REMOTE).
 */
Mode ui_get_current_mode();


#endif // UI_H
