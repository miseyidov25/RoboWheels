#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>

#include "bt.h"        // uses Mode enum + currentMode
#include "motors.h"    // for currentEffectiveSpeed()
#include "oled.h"

// OLED 128x64 I2C
static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0,
  U8X8_PIN_NONE
);

// --- UI STATE ---
static Mode menuSelection = MANUAL;
static bool menuActive = false;
static bool hasSelectedMode = false;

// Buttons for menu navigation
static const uint8_t BTN_NEXT = A1;
static const uint8_t BTN_SELECT = A2;

static bool lastNext = HIGH;
static bool lastSelect = HIGH;

// Runtime counter
static uint32_t totalSeconds = 0;
static unsigned long lastSecond = 0;

// EEPROM
static const int EEPROM_ADDR = 0;

// --------- Utility ----------
static void formatTime(uint32_t sec, char* buf)
{
    uint32_t h = sec / 3600;
    uint32_t m = (sec % 3600) / 60;
    uint32_t s = sec % 60;
    sprintf(buf, "%02lu:%02lu:%02lu", h, m, s);
}

// --------- Information from ROBOT motors ----------
static const char* getDirection()
{
    int s = currentEffectiveSpeed();

    if (s == 0)
        return "Stopped";

    // crude but functional inference based on pin states:
    // For simplicity: You may refine later
    return "Moving";
}

static int getSpeedPWM()
{
    return currentEffectiveSpeed();
}

// --------- Drawing screens ----------
static void drawStatus()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x13_tr);

    u8g2.setCursor(0, 12);
    u8g2.print("Mode: ");
    switch (currentMode) {
        case MANUAL:      u8g2.print("Manual"); break;
        case AUTONOMOUS:  u8g2.print("Auto");   break;
        case SLAVE:       u8g2.print("Slave");  break;
    }

    u8g2.setCursor(0, 28);
    u8g2.print("Dir : ");
    u8g2.print(getDirection());

    u8g2.setCursor(0, 44);
    u8g2.print("Speed: ");
    u8g2.print(getSpeedPWM());

    char buf[16];
    formatTime(totalSeconds, buf);

    u8g2.setCursor(0, 60);
    u8g2.print("Time: ");
    u8g2.print(buf);

    u8g2.sendBuffer();
}

static void drawMenu()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x13_tr);

    u8g2.setCursor(0, 12);
    u8g2.print("Select Mode:");

    const char* names[3] = {"Manual", "Autonomous", "Slave"};

    for (int m = 0; m < 3; m++)
    {
        int y = 30 + m * 15;
        if (menuSelection == (Mode)m) {
            u8g2.setCursor(0, y);
            u8g2.print(">");
        }

        u8g2.setCursor(12, y);
        u8g2.print(names[m]);
    }

    u8g2.sendBuffer();
}

// --------- Input ---------
static void handleButtons()
{
    bool n = digitalRead(BTN_NEXT);
    bool s = digitalRead(BTN_SELECT);

    // NEXT scrolls when menu active
    if (menuActive && lastNext == HIGH && n == LOW) {
        menuSelection = (Mode)(((int)menuSelection + 1) % 3);
    }

    // SELECT toggles menu or confirms
    if (lastSelect == HIGH && s == LOW) {
        if (menuActive) {
            currentMode = menuSelection;
            hasSelectedMode = true;
            menuActive = false;
        } else {
            menuActive = true;
            menuSelection = currentMode;
        }
    }

    lastNext = n;
    lastSelect = s;
}

// --------- Runtime update ----------
static void updateTime()
{
    if (!hasSelectedMode) return;

    unsigned long now = millis();
    if (now - lastSecond >= 1000) {
        lastSecond = now;
        totalSeconds++;
    }
}

// --------- Public Functions ----------
void oled_init()
{
    pinMode(BTN_NEXT, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);

    u8g2.begin();

    menuActive = true;
    hasSelectedMode = false;
}

void oled_update()
{
    updateTime();
    handleButtons();

    if (menuActive)
        drawMenu();
    else
        drawStatus();
}
