#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include "ui.h" // Our custom header file

// ===================== PIN CONFIGURATION =====================

// Buttons
static const uint8_t BTN_NEXT_PIN   = 2;   // Button to cycle through modes
static const uint8_t BTN_SELECT_PIN = 3;   // Button to enter/confirm selection

// OLED: standard I2C SSD1306 128x64 on A4(SDA), A5(SCL)
static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
    U8G2_R0,        // rotation
    U8X8_PIN_NONE   // reset pin
);

// ===================== INTERNAL STATE VARIABLES =====================

static Mode currentMode   = MODE_AUTONOMOUS;   // Active mode
static Mode menuSelection = MODE_AUTONOMOUS;   // Highlighted in menu

static bool menuActive      = false;   // true = menu, false = status
static bool hasSelectedMode = false;   // did we ever select a mode?

// Button edge detection (for INPUT_PULLUP, so idle = HIGH, pressed = LOW)
static bool lastNextState   = HIGH;
static bool lastSelectState = HIGH;

// Runtime (total active time in seconds)
static uint32_t totalSeconds = 0;

// For millis() timing
static unsigned long lastSecondTick = 0;

// EEPROM address for saving totalSeconds (4 bytes)
static const int EEPROM_ADDR_TOTAL_SECONDS = 0;
static bool eepromDirty = false;
static unsigned long lastEepromWrite = 0;
static const unsigned long EEPROM_WRITE_INTERVAL_MS = 10000UL; // write every 10s


// ===================== FORWARD DECLARATIONS (Internal Functions) =====================

static const char* modeToString(Mode m);
static void drawMenuScreen();
static void drawStatusScreen();
static void readButtons();
static void updateRuntime();
static void saveTotalSecondsToEEPROM();


// ===================== PLACEHOLDERS: DIRECTION & SPEED =====================
// These functions are only used by the UI module for display.
// TODO: Connect them to your robot's actual logic.

static const char* getCurrentDirection()
{
  // e.g. "Forward", "Backward", "Left", "Right", "Stopped"
  return "Forward";
}

static int getCurrentSpeed()
{
  // e.g. PWM 0–255
  return 128; // placeholder
}

// ===================== HELPER FUNCTIONS =====================

static const char* modeToString(Mode m)
{
  switch (m) {
    case MODE_AUTONOMOUS: return "Autonomous";
    case MODE_SLAVE:      return "Slave";
    case MODE_REMOTE:     return "Remote";
    default:              return "Unknown";
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

// ===================== EEPROM HANDLING =====================

static void loadTotalSecondsFromEEPROM()
{
  uint32_t value = 0;
  for (int i = 0; i < 4; ++i) {
    uint8_t b = EEPROM.read(EEPROM_ADDR_TOTAL_SECONDS + i);
    value |= ((uint32_t)b << (8 * i));
  }
  totalSeconds = value;
}

static void saveTotalSecondsToEEPROM()
{
  uint32_t value = totalSeconds;
  for (int i = 0; i < 4; ++i) {
    uint8_t b = (value >> (8 * i)) & 0xFF;
    EEPROM.update(EEPROM_ADDR_TOTAL_SECONDS + i, b);
  }
}

// ===================== DRAWING FUNCTIONS =====================

static void drawStatusScreen()
{
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x13_tr);

  // 1) Top line: active mode
  u8g2.setCursor(0, 12);
  u8g2.print("Mode: ");
  u8g2.print(modeToString(currentMode));

  // 2) Direction
  u8g2.setCursor(0, 28);
  u8g2.print("Dir : ");
  u8g2.print(getCurrentDirection());

  // 3) Speed
  u8g2.setCursor(0, 44);
  u8g2.print("Spd : ");
  u8g2.print(getCurrentSpeed());
  u8g2.print(" (PWM)");

  // 4) Time
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

  // Title
  u8g2.setCursor(0, 12);
  if (!hasSelectedMode) {
    u8g2.print("Select mode to start:");
  } else {
    u8g2.print("Select mode:");
  }

  // List of modes with arrow for selected
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
  // Only in menu: cycle through modes
  if (menuActive) {
    int idx = (int)menuSelection;
    idx = (idx + 1) % MODE_COUNT;   // 0→1→2→0
    menuSelection = (Mode)idx;
  }
}

static void handleSelectPressed()
{
  if (menuActive) {
    // CONFIRM selection → activate mode + go to status
    currentMode     = menuSelection;
    hasSelectedMode = true;
    menuActive      = false;
  } else {
    // We are on status screen → re-open menu
    menuActive    = true;
    menuSelection = currentMode;
  }
}

static void readButtons()
{
  bool currentNext   = digitalRead(BTN_NEXT_PIN);
  bool currentSelect = digitalRead(BTN_SELECT_PIN);

  // Active LOW (INPUT_PULLUP): press = HIGH -> LOW
  if (lastNextState == HIGH && currentNext == LOW) {
    handleNextPressed();
  }

  if (lastSelectState == HIGH && currentSelect == LOW) {
    handleSelectPressed();
  }

  lastNextState   = currentNext;
  lastSelectState = currentSelect;
}

// ===================== TIME & LOOP LOGIC =====================

static void updateRuntime()
{
  // This function only tracks time when a mode is active
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

// ===================== PUBLIC FUNCTIONS (Definitions) =====================

void ui_init()
{
  // Buttons
  pinMode(BTN_NEXT_PIN,   INPUT_PULLUP);
  pinMode(BTN_SELECT_PIN, INPUT_PULLUP);

  // OLED
  u8g2.begin();

  // Load total time from EEPROM
  loadTotalSecondsFromEEPROM();

  // Initial button states
  lastNextState   = digitalRead(BTN_NEXT_PIN);
  lastSelectState = digitalRead(BTN_SELECT_PIN);

  // Optional splash
  drawSplashScreen();
  delay(1500);

  // REQUIREMENT: at first it must have mode selection
  menuActive      = true;           // start in menu
  hasSelectedMode = false;          // no mode chosen yet
  currentMode     = MODE_AUTONOMOUS; // default mode
  menuSelection   = currentMode;     // cursor on default
}

void ui_update()
{
  updateRuntime();
  readButtons();

  // Always show menu until user selects a mode once.
  if (menuActive) {
    drawMenuScreen();
  } else {
    drawStatusScreen();
  }

  delay(20); // Small delay to debounce and save power
}

Mode ui_get_current_mode()
{
  // If no mode is selected yet, return the default.
  // Or you could define a "NONE" or "MENU" state.
  // Here, we only consider the mode valid when the menu is inactive.
  if (menuActive) {
      // If we are in the menu, return the last active mode
      // but you might signal the robot to do nothing (e.g., by returning MODE_COUNT)
      // For now, just return the selected mode.
      return currentMode;
  }
  return currentMode;
}
