#ifndef OLED_H
#define OLED_H


void drawMenuScreen();
void drawStatusScreen();
void drawSplashScreen();
void readButtons();
void updateRuntime();
void loadTotalSecondsFromEEPROM();
void saveTotalSecondsToEEPROM();

void oled_init();
void oled_update();



#endif
