#ifndef BT_H
#define BT_H

#include <stdbool.h>

// Declare Mode enum ONCE for the whole project:
typedef enum {
    NONE,       // no active mode, used to return to menu
    MANUAL,
    AUTONOMOUS,
    SLAVE,
    COUNT       // total number of selectable modes
} Mode;

// Declare the global variable (but DO NOT define it)
extern Mode currentMode;

// Speed index variables from bt_module
extern int currentSpeedIndex;
extern int speedLevels[];
extern const int speedLevelsCount;

// UART functions
void uart_init();
void uart_print(const char* str);
void uart_println(const char* str);
void uart_print_int(int num);
void uart_putc(char c);
void uart_puts(const char* str);

char bt_get_active_cmd();

// Bluetooth functions
void bt_init();
void bt_update();

#endif
