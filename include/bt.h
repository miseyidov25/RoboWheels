#ifndef BT_H
#define BT_H

// Declare Mode enum ONCE for the whole project:
enum Mode { MANUAL, AUTONOMOUS, SLAVE, COUNT };

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

#endif
