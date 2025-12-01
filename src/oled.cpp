#include <Arduino.h>
#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>

// ===================== ENUMS =====================

// Different operation modes of the robot
enum Mode {
  MODE_AUTONOMOUS = 0,
  MODE_SLAVE      = 1,
  MODE_REMOTE     = 2,
  MODE_COUNT      = 3   // number of modes
};

// ===================== PIN CONFIGURATION =====================

// Button to move up/down through the menu (ONLY navigation)
static const uint8_t BTN_NEXT_PIN   = A2;

// Button to select a mode and open/close the menu
static const uint8_t BTN_SELECT_PIN = A3;

// OLED: standard I2C SSD1306 128x64 on A4(SDA), A5(SCL)
static U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(
  U8G2_R0,        // rotation
  U8X8_PIN_NONE   // no reset pin
);

// ===================== UI STATE =====================

// Currently active mode (the mode the robot should run)
static Mode currentMode   = MODE_AUTONOMOUS;

// Highlighted mode in the menu (the arrow position)
static Mode menuSelection = MODE_AUTONOMOUS;

// true  = menu screen is visible
// false = status screen is visible
static bool menuActive      = false;

// true after a mode has been selected at least once
static bool hasSelectedMode = false;

// Last sampled button states (for edge detection)
static bool lastNextState   = HIGH;  // for BTN_NEXT_PIN
static bool lastSelectState = HIGH;  // for BTN_SELECT_PIN

// ===================== RUNTIME / EEPROM =====================

// Total runtime (seconds) while a mode is active
static uint32_t totalSeconds = 0;

// For timing with millis()
static unsigned long lastSecondTick = 0;

// EEPROM address where totalSeconds (4 bytes) is stored
static const int EEPROM_ADDR_TOTAL_SECONDS = 0;

// Flags and timing to avoid writing to EEPROM too often
static bool eepromDirty = false;
static unsigned long lastEepromWrite = 0;
static const unsigned long EEPROM_WRITE_INTERVAL_MS = 10000UL; // 10 seconds

// ===================== FORWARD DECLARATIONS =====================

static void drawMenuScreen();
static void drawStatusScreen();
static void drawSplashScreen();
static void readButtons();
static void updateRuntime();
static void loadTotalSecondsFromEEPROM();
static void saveTotalSecondsToEEPROM();
static const char* modeToString(Mode m);

void ui_init();
void ui_update();
Mode ui_get_current_mode();

// ===================== PLACEHOLDERS: DIRECTION & SPEED =====================
// TODO: Replace these with real data from your motor logic.

static const char* getCurrentDirection()
{
  // Example values later: "Forward", "Backward", "Left", "Right", "Stopped"
  return "Forward";
}

static int getCurrentSpeed()
{
  // Example: PWM value 0–255
  return 128;
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

  // Line 1: active mode
  u8g2.setCursor(0, 12);
  u8g2.print("Mode: ");
  u8g2.print(modeToString(currentMode));

  // Line 2: direction
  u8g2.setCursor(0, 28);
  u8g2.print("Dir : ");
  u8g2.print(getCurrentDirection());

  // Line 3: speed
  u8g2.setCursor(0, 44);
  u8g2.print("Spd : ");
  u8g2.print(getCurrentSpeed());
  u8g2.print(" (PWM)");

  // Line 4: total runtime
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

  // Title line
  u8g2.setCursor(0, 12);
  if (!hasSelectedMode) {
    u8g2.print("Select mode to start:");
  } else {
    u8g2.print("Select mode:");
  }

  // List of modes with arrow on selected line
  int startY = 28;
  const int lineHeight = 14;

  for (int i = 0; i < MODE_COUNT; ++i) {
    int y = startY + i * lineHeight;

    if ((int)menuSelection == i) {
      // Draw cursor arrow
      u8g2.setCursor(0, y);
      u8g2.print(">");
    }

    // Draw mode name
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

// NEXT button (pin 2): ONLY scrolls through menu items
static void handleNextPressed()
{
  // Only do something when the menu is visible
  if (menuActive) {
    int idx = (int)menuSelection;
    idx = (idx + 1) % MODE_COUNT;   // 0→1→2→0
    menuSelection = (Mode)idx;
  }
}

// SELECT button (pin 3): selects mode or opens/closes menu
static void handleSelectPressed()
{
  if (menuActive) {
    // We are in the menu: confirm selection and go to status screen
    currentMode     = menuSelection;
    hasSelectedMode = true;
    menuActive      = false;
  } else {
    // We are in the status screen: reopen the menu
    menuActive    = true;
    menuSelection = currentMode;  // start cursor at current mode
  }
}

static void readButtons()
{
  bool currentNext   = digitalRead(BTN_NEXT_PIN);
  bool currentSelect = digitalRead(BTN_SELECT_PIN);

  // Buttons use INPUT_PULLUP: idle = HIGH, pressed = LOW
  // We detect the falling edge: HIGH -> LOW

  // PIN 2: NEXT (ONLY scrolls in menu, never selects)
  if (menuActive) {
    if (lastNextState == HIGH && currentNext == LOW) {
      handleNextPressed();
    }
  }
  // When menuActive == false, pin 2 does NOTHING at all

  // PIN 3: SELECT (always allowed to open/close/select)
  if (lastSelectState == HIGH && currentSelect == LOW) {
    handleSelectPressed();
  }

  lastNextState   = currentNext;
  lastSelectState = currentSelect;
}

// ===================== RUNTIME & EEPROM UPDATE =====================

static void updateRuntime()
{
  // Only count runtime after a mode has been selected at least once
  if (!hasSelectedMode) return;

  unsigned long now = millis();

  // Increment seconds
  if (now - lastSecondTick >= 1000UL) {
    lastSecondTick += 1000UL;
    totalSeconds++;
    eepromDirty = true;
  }

  // Save to EEPROM every EEPROM_WRITE_INTERVAL_MS when changed
  if (eepromDirty && (now - lastEepromWrite >= EEPROM_WRITE_INTERVAL_MS)) {
    saveTotalSecondsToEEPROM();
    lastEepromWrite = now;
    eepromDirty     = false;
  }
}

// ===================== UI PUBLIC API =====================

void ui_init()
{
  // Configure buttons with internal pull-ups
  pinMode(BTN_NEXT_PIN,   INPUT_PULLUP);
  pinMode(BTN_SELECT_PIN, INPUT_PULLUP);

  // Initialize OLED
  u8g2.begin();

  // Load total runtime from EEPROM
  loadTotalSecondsFromEEPROM();

  // Seed last button states
  lastNextState   = digitalRead(BTN_NEXT_PIN);
  lastSelectState = digitalRead(BTN_SELECT_PIN);

  // Show splash screen
  drawSplashScreen();
  delay(1500);

  // Start in menu; user must choose a mode before runtime counts
  menuActive      = true;
  hasSelectedMode = false;
  currentMode     = MODE_AUTONOMOUS;
  menuSelection   = currentMode;
}

void ui_update()
{
  updateRuntime();
  readButtons();

  if (menuActive) {
    drawMenuScreen();
  } else {
    drawStatusScreen();
  }

  delay(20); // small delay for debouncing and lower CPU usage
}

Mode ui_get_current_mode()
{
  return currentMode;
}

// ===================== ARDUINO ENTRY POINTS =====================

void setup()
{
  ui_init();

  // TODO: Initialize other subsystems here if needed:
  // motor_init();
  // sensors_init();
  // bt_init();
}

void loop()
{
  ui_update();

  // Example place to act on the current mode:
  /*
  switch (ui_get_current_mode()) {
    case MODE_AUTONOMOUS:
      // autonomous_control_step();
      break;
    case MODE_SLAVE:
      // slave_control_step();
      break;
    case MODE_REMOTE:
      // remote_control_step();
      break;
    default:
      break;
  }
  */
}