#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include "bt.h"
#include "oled.h"
#include "motors.h"
#include "pins.h"
#include "line.h"

// PIN CONFIG

static const uint8_t BTN_NEXT_PIN   = A2;
static const uint8_t BTN_SELECT_PIN = A3;

// OLED: SSD1306 128x64 on A4/SDA, A5/SCL
static U8G2_SSD1306_128X64_NONAME_1_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// UI STATE 

static Mode menuSelection = MANUAL;
bool menuActive = false;
bool hasSelectedMode = false;

// Last sampled button states
static bool lastNextState   = HIGH;
static bool lastSelectState = HIGH;

// RUNTIME/EEPROM 

static uint32_t totalSeconds = 0;
static unsigned long lastSecondTick = 0;
static const int EEPROM_ADDR_TOTAL_SECONDS = 0;
static bool eepromDirty = false;
static unsigned long lastEepromWrite = 0;
static const unsigned long EEPROM_WRITE_INTERVAL_MS = 5000UL;

// FORWARD DECLARATIONS 

static const char* modeToString(Mode m);

void oled_init();
void oled_update();
Mode ui_get_current_mode();

// HELPER FUNCTIONS 

static const char* getCurrentDirection(void)
{
    switch (motorDirection)
    {
        case MOTOR_FORWARD:       return "Fwd";
        case MOTOR_REVERSE:       return "Rev";
        case MOTOR_LEFT:          return "Left";
        case MOTOR_RIGHT:         return "Right";
        case MOTOR_CORRECT_LEFT:  return "Corr L";
        case MOTOR_CORRECT_RIGHT: return "Corr R";
        default:                  return "Stop";
    }
}
    

static int getCurrentSpeed() {
    return currentEffectiveSpeed();
}

static const char* modeToString(Mode m) {
    switch (m) {
        case AUTONOMOUS: return "Auto";
        case SLAVE:      return "Slave";
        case MANUAL:     return "Manual";
        default:         return "?";
    }
}

void formatTime(uint32_t seconds, char* buffer, size_t len) {
    uint32_t h = seconds / 3600;
    uint32_t m = (seconds % 3600) / 60;
    uint32_t s = seconds % 60;
    snprintf(buffer, len, "%02lu:%02lu:%02lu", (unsigned long)h, (unsigned long)m, (unsigned long)s);
}

// EEPROM HANDLING 

void loadTotalSecondsFromEEPROM() {
    uint32_t value = 0;
    for (int i = 0; i < 4; ++i) {
        uint8_t b = EEPROM.read(EEPROM_ADDR_TOTAL_SECONDS + i);
        value |= ((uint32_t)b << (8 * i));
    }
    totalSeconds = value;
}

void saveTotalSecondsToEEPROM() {
    uint32_t value = totalSeconds;
    for (int i = 0; i < 4; ++i) {
        uint8_t b = (value >> (8 * i)) & 0xFF;
        EEPROM.update(EEPROM_ADDR_TOTAL_SECONDS + i, b);
    }
}

// DRAWING FUNCTIONS 

void drawStatusScreen() {
    u8g2.setFont(u8g2_font_6x13_tr);

    u8g2.firstPage();
    do {
        char buf[40];

        snprintf(buf, sizeof(buf), "Mode: %s", modeToString(currentMode));
        u8g2.drawStr(0, 12, buf);

        snprintf(buf, sizeof(buf), "Dir : %s", getCurrentDirection());
        u8g2.drawStr(0, 28, buf);

        snprintf(buf, sizeof(buf), "Spd : %d", getCurrentSpeed());
        u8g2.drawStr(0, 44, buf);

        char timebuf[16];
        formatTime(totalSeconds, timebuf, sizeof(timebuf));
        snprintf(buf, sizeof(buf), "Time: %s", timebuf);
        u8g2.drawStr(0, 62, buf);

    } while (u8g2.nextPage());
}

void drawMenuScreen() {
    u8g2.setFont(u8g2_font_6x13_tr);
    u8g2.firstPage();
    do {
        u8g2.drawStr(0, 12, "Select mode:");

        int startY = 28;
        const int lineHeight = 14;

        // Only show real modes: MANUAL, AUTONOMOUS, SLAVE
        for (int i = MANUAL; i <= SLAVE; ++i) {
            int y = startY + (i - MANUAL) * lineHeight;

            char buf[20];
            if (menuSelection == (Mode)i)
                snprintf(buf, sizeof(buf), "> %s", modeToString((Mode)i));
            else
                snprintf(buf, sizeof(buf), "  %s", modeToString((Mode)i));

            u8g2.drawStr(0, y, buf);
        }

    } while (u8g2.nextPage());
}

void drawSplashScreen() {
    u8g2.setFont(u8g2_font_6x13_tr);

    u8g2.firstPage();
    do {
        u8g2.drawStr(0, 24, "RoboWheel");
        u8g2.drawStr(0, 40, "Mode Selection First");
    } while (u8g2.nextPage());
}

// BUTTON HANDLING 

static void handleNextPressed() {
    if (!menuActive) return;

    int idx = (int)menuSelection;
    idx++;
    if (idx > SLAVE) idx = MANUAL;  // wrap around
    menuSelection = (Mode)idx;
    u8g2.clearDisplay();
}

static void handleSelectPressed() {
    if (menuActive) {
        currentMode = menuSelection;
        hasSelectedMode = true;
        menuActive = false;
        u8g2.clearDisplay();
        Serial.print("Mode selected: ");
        Serial.println(modeToString(currentMode));
    } else {
        menuActive = true;
        menuSelection = currentMode;
        u8g2.clearDisplay();
        Serial.println("Menu reopened");
    }
}

void readButtons() {
    bool currentNext = digitalRead(BTN_NEXT_PIN);
    bool currentSelect = digitalRead(BTN_SELECT_PIN);

    // NEXT button
    if (menuActive && lastNextState == HIGH && currentNext == LOW) {
        Serial.println("NEXT button pressed");
        handleNextPressed();
    }

    // SELECT button
    if (lastSelectState == HIGH && currentSelect == LOW) {
        Serial.println("SELECT button pressed");
        handleSelectPressed();
    }

    lastNextState = currentNext;
    lastSelectState = currentSelect;
}

// RUNTIME & EEPROM UPDATE 

void updateRuntime() {
    if (!hasSelectedMode) return;

    unsigned long now = millis();

    // Increment seconds
    if (now - lastSecondTick >= 1000UL) {
        lastSecondTick += 1000UL;
        totalSeconds++;
        eepromDirty = true;
    }

    // EEPROM save
    if (eepromDirty && now - lastEepromWrite >= EEPROM_WRITE_INTERVAL_MS) {
        saveTotalSecondsToEEPROM();
        lastEepromWrite = now;
        eepromDirty = false;
    }
}

// UI PUBLIC API

void oled_init() {
    pinMode(BTN_NEXT_PIN, INPUT_PULLUP);
    pinMode(BTN_SELECT_PIN, INPUT_PULLUP);

    u8g2.begin();
    loadTotalSecondsFromEEPROM();

    lastNextState = digitalRead(BTN_NEXT_PIN);
    lastSelectState = digitalRead(BTN_SELECT_PIN);

    drawSplashScreen();
    delay(1500);

    menuActive = true;
    hasSelectedMode = false;
    currentMode = MANUAL;
    menuSelection = currentMode;
}

void oled_update() {
    updateRuntime();
    readButtons();

    if (menuActive)
        drawMenuScreen();
    else {
        static unsigned long lastPrint = 0;
        if (millis() - lastPrint >= 1000) {
            char buf[16];
            formatTime(totalSeconds, buf, sizeof(buf));
            Serial.print("Runtime: ");
            Serial.println(buf);
            lastPrint = millis();
        }
        drawStatusScreen();
    }

    delay(20);
}

Mode ui_get_current_mode() {
    return currentMode;
}
