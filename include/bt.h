#ifndef BT_H
#define BT_H

// Declare Mode enum ONCE for the whole project:
enum Mode {
    NONE,       // no active mode, used to return to menu
    MANUAL,
    AUTONOMOUS,
    SLAVE,
    COUNT       // total number of selectable modes
};

// Declare the global variable (but DO NOT define it)
extern Mode currentMode;

// Speed index variables from bt_module
extern int currentSpeedIndex;
extern int speedLevels[];
extern const int speedLevelsCount;

// Functions
void bt_init();
void bt_update();
void executeCommand(char cmd);

char bt_get_active_cmd();

#endif
