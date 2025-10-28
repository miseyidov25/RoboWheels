#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

// Button Pins
#define BTN_FWD 11
#define BTN_REV 12
#define BTN_LFT 10
#define BTN_RST 21

// Bluetooth icon bitmap
static const unsigned char bluetooth_bmp[] U8X8_PROGMEM = {
  0b00011000, 0b00100100, 0b01011010, 0b00011000,
  0b00011000, 0b01011010, 0b00100100, 0b00011000
};

// Menu states
#define MENU_MAIN 0
#define MODE_A 1
#define MODE_B 2
#define MODE_C 3

// Button struct
struct Button {
  uint8_t pin;
  uint8_t last_state;
  uint8_t current_state;
  unsigned long last_change_time;
};

Button btn_fwd = {BTN_FWD, HIGH, HIGH, 0};
Button btn_rev = {BTN_REV, HIGH, HIGH, 0};
Button btn_lft = {BTN_LFT, HIGH, HIGH, 0};
Button btn_rst = {BTN_RST, HIGH, HIGH, 0};

// Global state
uint8_t current_menu = MENU_MAIN;
uint8_t selected_mode = 0;
uint8_t menu_cursor = 1;  // Cursor position in menu (1=A, 2=B, 3=C)
bool mode_locked = false;  // Once mode is selected, it's locked
uint32_t runtime = 0;
unsigned long last_tick = 0;
unsigned long last_save = 0;
bool needs_save = false;
bool bt_connected = true;
uint8_t speed_percent = 45;  // Random speed value

const unsigned long DEBOUNCE_DELAY = 20;
const unsigned long SAVE_INTERVAL = 60000;
const int EEPROM_ADDR = 0;

// ===== INITIALIZATION =====

void oled_init() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.clearBuffer();
  u8g2.drawStr(15, 30, "Init...");
  u8g2.sendBuffer();
  delay(1000);
}

void setup_pins() {
  pinMode(BTN_FWD, INPUT_PULLUP);
  pinMode(BTN_REV, INPUT_PULLUP);
  pinMode(BTN_LFT, INPUT_PULLUP);
  pinMode(BTN_RST, INPUT_PULLUP);
}

// ===== BUTTON HANDLING =====

bool button_pressed(Button &btn) {
  int reading = digitalRead(btn.pin);

  if (reading != btn.last_state) {
    btn.last_change_time = millis();
  }

  if ((millis() - btn.last_change_time) > DEBOUNCE_DELAY) {
    if (reading != btn.current_state) {
      btn.current_state = reading;
      if (btn.current_state == LOW) {
        btn.last_state = reading;
        return true;
      }
    }
  }

  btn.last_state = reading;
  return false;
}

// ===== DISPLAY FUNCTIONS =====

void draw_main_menu() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(25, 12, "SELECT MODE");
  u8g2.drawLine(0, 14, 128, 14);

  // Mode A with cursor
  if (menu_cursor == 1) {
    u8g2.drawBox(10, 26, 90, 10);
    u8g2.setDrawColor(0);
    u8g2.drawStr(15, 32, "Mode A");
    u8g2.setDrawColor(1);
  } else {
    u8g2.drawStr(15, 32, "Mode A");
  }

  // Mode B with cursor
  if (menu_cursor == 2) {
    u8g2.drawBox(10, 39, 90, 10);
    u8g2.setDrawColor(0);
    u8g2.drawStr(15, 45, "Mode B");
    u8g2.setDrawColor(1);
  } else {
    u8g2.drawStr(15, 45, "Mode B");
  }

  // Mode C with cursor
  if (menu_cursor == 3) {
    u8g2.drawBox(10, 52, 90, 10);
    u8g2.setDrawColor(0);
    u8g2.drawStr(15, 58, "Mode C");
    u8g2.setDrawColor(1);
  } else {
    u8g2.drawStr(15, 58, "Mode C");
  }

  u8g2.sendBuffer();
}

void draw_mode_screen(uint8_t mode) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);

  if (bt_connected) {
    u8g2.drawXBMP(0, 0, 8, 8, bluetooth_bmp);
    u8g2.drawStr(12, 8, "BT OK");
  } else {
    u8g2.drawStr(0, 8, "BT No");
  }
  u8g2.drawLine(0, 10, 128, 10);

  u8g2.drawStr(5, 25, "Mode: ");
  u8g2.setFont(u8g2_font_8x13_tf);
  if (mode == 1) u8g2.drawStr(40, 28, "A");
  else if (mode == 2) u8g2.drawStr(40, 28, "B");
  else if (mode == 3) u8g2.drawStr(40, 28, "C");

  u8g2.setFont(u8g2_font_6x12_tf);
  char buf[20];
  snprintf(buf, sizeof(buf), "Time: %lus", runtime);
  u8g2.drawStr(5, 40, buf);

  snprintf(buf, sizeof(buf), "Speed: %d%%", speed_percent);
  u8g2.drawStr(5, 52, buf);

  //u8g2.drawStr(30, 64, "RST -> Menu");
  u8g2.sendBuffer();
}

// ===== STATE HANDLING =====

void handle_input() {
  if (button_pressed(btn_fwd)) {
    if (current_menu == MENU_MAIN) {
      // Navigate down through modes
      if (menu_cursor < 3) menu_cursor++;
      Serial.println("FWD - Navigate Down");
    }
  }

  if (button_pressed(btn_rev)) {
    if (current_menu == MENU_MAIN) {
      // Navigate up through modes
      if (menu_cursor > 1) menu_cursor--;
      Serial.println("REV - Navigate Up");
    }
  }

  if (button_pressed(btn_lft)) {
    if (current_menu == MENU_MAIN) {
      // Select current mode
      selected_mode = menu_cursor;
      current_menu = menu_cursor;
      Serial.print("LFT - Select Mode: ");
      Serial.println(menu_cursor);
    }
  }

  if (button_pressed(btn_rst)) {
    // Always return to menu
    Serial.println("RST - Back to Menu");
    current_menu = MENU_MAIN;
    selected_mode = 0;
    menu_cursor = 1;
    runtime = 0;
  }
}

void update_display() {
  if (current_menu == MENU_MAIN) {
    draw_main_menu();
  } else {
    draw_mode_screen(current_menu);
  }
}

// ===== EEPROM =====

void save_runtime(uint32_t value) {
  uint32_t saved;
  EEPROM.get(EEPROM_ADDR, saved);
  if (abs((long)(value - saved)) > 1) {
    EEPROM.put(EEPROM_ADDR, value);
  }
}

uint32_t load_runtime() {
  uint32_t saved = 0;
  EEPROM.get(EEPROM_ADDR, saved);
  if (saved == 0xFFFFFFFF || saved > 864000) {
    saved = 0;
    EEPROM.put(EEPROM_ADDR, saved);
  }
  return saved;
}

// ===== MAIN =====

void setup() {
  Serial.begin(9600);
  setup_pins();
  oled_init();
  runtime = load_runtime();
}

void loop() {
  unsigned long now = millis();

  handle_input();

  if (current_menu != MENU_MAIN && (now - last_tick >= 1000)) {
    runtime++;
    last_tick = now;
    needs_save = true;
  }

  if (needs_save && (now - last_save >= SAVE_INTERVAL)) {
    save_runtime(runtime);
    last_save = now;
    needs_save = false;
  }

  update_display();
  delay(50);
}
