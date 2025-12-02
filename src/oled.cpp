
#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include "bt.h"
#include "oled.h"
#include "motors.h"
#include "pins.h"
#include "line.h"

// ===================== PIN CONFIG =====================

static const uint8_t BTN_NEXT_PIN   = A2;
static const uint8_t BTN_SELECT_PIN = A3;

// Use page-buffer mode (_1_HW_I2C) to save ~1KB RAM
static U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(
    U8G2_R0,
    U8X8_PIN_NONE
);

// ===================== UI STATE =====================

// USE THE GLOBAL currentMode from main.cpp (DO NOT REDEFINE IT)
extern Mode currentMode;

static Mode menuSelection = AUTONOMOUS;
#define MODE_COUNT 3

static bool menuActive      = false;
static bool hasSelectedMode = false;

static bool lastNextState   = HIGH;
static bool lastSelectState = HIGH;

// ===================== RUNTIME / EEPROM =====================

static uint32_t totalSeconds = 0;
static unsigned long lastSecondTick = 0;

static const int EEPROM_ADDR_TOTAL_SECONDS = 0;

static bool eepromDirty = false;
static unsigned long lastEepromWrite = 0;
static const unsigned long EEPROM_WRITE_INTERVAL_MS = 10000UL;

// ===================== FORWARD DECLARATIONS =====================

static void drawMenuScreen();
static void drawStatusScreen();
static void drawSplashScreen();
static void readButtons();
static void updateRuntime();
static void loadTotalSecondsFromEEPROM();
static void saveTotalSecondsToEEPROM();
static const char* modeToString(Mode m);

// ===================== STATUS FUNCTIONS =====================

static const char* getCurrentDirection() {
    switch (motorDirection) {
        case 1:  return "Forward";
        case -1: return "Reverse";
        case 2:  return "Left";
        case 3:  return "Right";
        default: return "Stopped";
    }
}

static int getCurrentSpeed() {
    return currentEffectiveSpeed();
}

// ===================== HELPERS =====================

static const char* modeToString(Mode m)
{
    switch (m) {
        case AUTONOMOUS: return "Autonomous";
        case SLAVE:      return "Slave";
        case MANUAL:     return "Manual";
        default:         return "Unknown";
    }
}

static void formatTime(uint32_t seconds, char* buffer, size_t len)
{
    uint32_t h = seconds / 3600;
    uint32_t m = (seconds % 3600) / 60;
    uint32_t s = seconds % 60;

    snprintf(buffer, len, "%02lu:%02lu:%02lu",
             (unsigned long)h,
             (unsigned long)m,
             (unsigned long)s);
}

// ===================== EEPROM =====================

static void loadTotalSecondsFromEEPROM()
{
    uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        value |= ((uint32_t)EEPROM.read(EEPROM_ADDR_TOTAL_SECONDS + i)) << (8 * i);
    }
    totalSeconds = value;
}

static void saveTotalSecondsToEEPROM()
{
    uint32_t value = totalSeconds;
    for (int i = 0; i < 4; ++i) {
        EEPROM.update(EEPROM_ADDR_TOTAL_SECONDS + i, (value >> (8 * i)) & 0xFF);
    }
}

// ===================== DRAW SCREENS =====================

static void drawStatusScreen()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x13_tr);

    u8g2.setCursor(0, 12);
    u8g2.print("Mode: ");
    u8g2.print(modeToString(currentMode));

    u8g2.setCursor(0, 28);
    u8g2.print("Dir : ");
    u8g2.print(getCurrentDirection());

    u8g2.setCursor(0, 44);
    u8g2.print("Spd : ");
    u8g2.print(getCurrentSpeed());
    u8g2.print(" (PWM)");

    char buf[16];
    formatTime(totalSeconds, buf, sizeof(buf));
    u8g2.setCursor(0, 60);
    u8g2.print("Time: ");
    u8g2.print(buf);

    u8g2.sendBuffer();
}

static void drawMenuScreen()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x13_tr);

    u8g2.setCursor(0, 12);
    if (!hasSelectedMode) u8g2.print("Select mode to start:");
    else                  u8g2.print("Select mode:");

    int startY = 28;
    const int lineHeight = 14;

    for (int i = 0; i < MODE_COUNT; ++i) {
        int y = startY + i * lineHeight;

        if ((int)menuSelection == i) {
            u8g2.setCursor(0, y);
            u8g2.print(">");
        }

        u8g2.setCursor(12, y);
        u8g2.print(modeToString((Mode)i));
    }

    u8g2.sendBuffer();
}

static void drawSplashScreen()
{
    u8g2.clearBuffer();
    u8g2.setFont(u8g2_font_6x13_tr);

    u8g2.setCursor(0, 24);
    u8g2.print("RoboWheel");

    u8g2.setCursor(0, 40);
    u8g2.print("Mode Selection First");

    u8g2.sendBuffer();
}

// ===================== BUTTON HANDLING =====================

static void handleNextPressed()
{
    if (menuActive) {
        int idx = (int)menuSelection;
        idx = (idx + 1) % MODE_COUNT;
        menuSelection = (Mode)idx;
    }
}

static void handleSelectPressed()
{
    if (menuActive) {
        currentMode     = menuSelection;
        hasSelectedMode = true;
        menuActive      = false;
    } else {
        menuActive     = true;
        menuSelection  = currentMode;
    }
}

static void readButtons()
{
    bool currentNext   = digitalRead(BTN_NEXT_PIN);
    bool currentSelect = digitalRead(BTN_SELECT_PIN);

    if (menuActive && lastNextState == HIGH && currentNext == LOW)
        handleNextPressed();

    if (lastSelectState == HIGH && currentSelect == LOW)
        handleSelectPressed();

    lastNextState   = currentNext;
    lastSelectState = currentSelect;
}

// ===================== RUNTIME =====================

static void updateRuntime()
{
    if (!hasSelectedMode) return;

    unsigned long now = millis();

    if (now - lastSecondTick >= 1000UL) {
        lastSecondTick += 1000UL;
        totalSeconds++;
        eepromDirty = true;
    }

    if (eepromDirty && (now - lastEepromWrite >= EEPROM_WRITE_INTERVAL_MS)) {
        saveTotalSecondsToEEPROM();
        lastEepromWrite = now;
        eepromDirty = false;
    }
}

// ===================== PUBLIC API =====================

void oled_init()
{
    pinMode(BTN_NEXT_PIN,   INPUT_PULLUP);
    pinMode(BTN_SELECT_PIN, INPUT_PULLUP);

    u8g2.begin();

    loadTotalSecondsFromEEPROM();

    lastNextState   = digitalRead(BTN_NEXT_PIN);
    lastSelectState = digitalRead(BTN_SELECT_PIN);

// Immediately show menu on boot
menuActive      = true;
hasSelectedMode = false;
menuSelection   = MANUAL;

drawSplashScreen();
delay(1500);
drawMenuScreen();   // Start directly in menu

}

void oled_update()
{
    updateRuntime();
    readButtons();

    if (menuActive) drawMenuScreen();
    else            drawStatusScreen();

    delay(20);
}

Mode oled_get_current_mode()
{
    return currentMode;
}

