#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>
#include "buttons.h"  

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);


// #define BTN_FWD 11
// #define BTN_REV 12
// #define BTN_LFT 10
// #define BTN_RST 21

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

// struct Button { ... };
// Button btn_fwd = {...};
// Button btn_rev = {...};
// Button btn_lft = {...};
// Button btn_rst = {...};

// Global state
uint8_t current_menu = MENU_MAIN;
uint8_t selected_mode = 0;
uint8_t menu_cursor = 1;
bool mode_locked = false;
uint32_t runtime = 0;
unsigned long last_tick = 0;
unsigned long last_save = 0;
bool needs_save = false;
bool bt_connected = true;
uint8_t speed_percent = 45;

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


void draw_main_menu() {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.drawStr(25, 12, "SELECT MODE");
  u8g2.drawLine(0, 14, 128, 14);
  
  if (menu_cursor == 1) {
    u8g2.drawBox(10, 26, 90, 10);
    u8g2.setDrawColor(0);
    u8g2.drawStr(15, 32, "Mode A");
    u8g2.setDrawColor(1);
  } else {
    u8g2.drawStr(15, 32, "Mode A");
  }
  
  if (menu_cursor == 2) {
    u8g2.drawBox(10, 39, 90, 10);
    u8g2.setDrawColor(0);
    u8g2.drawStr(15, 45, "Mode B");
    u8g2.setDrawColor(1);
  } else {
    u8g2.drawStr(15, 45, "Mode B");
  }
  
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
  
  u8g2.sendBuffer();
}

// ===== STATE HANDLING =====

void handle_input() {
  
  if (is_forward_pressed()) {
    if (current_menu == MENU_MAIN) {
      if (menu_cursor < 3) menu_cursor++;
      Serial.println("FWD - Navigate Down");
    }
  }
  
  if (is_reverse_pressed()) {  
    if (current_menu == MENU_MAIN) {
      if (menu_cursor > 1) menu_cursor--;
      Serial.println("REV - Navigate Up");
    }
  }
  
  if (is_left_pressed()) {  
    if (current_menu == MENU_MAIN) {
      selected_mode = menu_cursor;
      current_menu = menu_cursor;
      Serial.print("LFT - Select Mode: ");
      Serial.println(menu_cursor);
    }
  }
  

  
  static bool last_test_state = false;
  bool current_test = is_test_forward_active();
  
  if (current_test && !last_test_state) {  // edge detection
    Serial.println("RST - Back to Menu");
    current_menu = MENU_MAIN;
    selected_mode = 0;
    menu_cursor = 1;
    runtime = 0;
  }
  last_test_state = current_test;
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
  buttons_init(); 
  oled_init();
  runtime = load_runtime();
}

void loop() {
  unsigned long now = millis();
  
  buttons_update();  
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
