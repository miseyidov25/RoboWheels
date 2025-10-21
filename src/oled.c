#include <Wire.h>
#include <U8g2lib.h>
#include <EEPROM.h>

U8G2_SSD1306_128X64_NONAME_F_HW_I2C u8g2(U8G2_R0, U8X8_PIN_NONE);

#define LEDR 4  //red
#define LEDG 3  //green
#define LEDB 2  //blue
#define SPEED 9
#define BT_WIDTH 8
#define BT_HEIGHT 8
static const unsigned char bluetooth_bmp[] U8X8_PROGMEM = {
  0b00011000,
  0b00100100,
  0b01011010,
  0b00011000,
  0b00011000,
  0b01011010,
  0b00100100,
  0b00011000
};

const char* directions[] = { "Stop", "Forward", "Reverse", "Left", "Right" };
uint8_t direction = 1;        
uint8_t speed = 50;           
bool bt_connected = true;     
uint32_t runtime = 0;         
unsigned long last_tick = 0;
unsigned long last_save = 0;
bool needs_save = false;

const int EEPROM_ADDR = 0;
const unsigned long SAVE_INTERVAL = 60000; // Save every 60 seconds
const unsigned long MIN_SAVE_INTERVAL = 10000; // Minimum 10 seconds between saves

void oled_init() {
  u8g2.begin();
  u8g2.setFont(u8g2_font_6x12_tf);
  u8g2.clearBuffer();
  u8g2.drawStr(20, 30, "OLED Initialized...");
  u8g2.sendBuffer();
  delay(1000);
}

void oled_draw_status(uint8_t dir, uint8_t spd, uint32_t time, bool bt) {
  u8g2.clearBuffer();
  u8g2.setFont(u8g2_font_6x12_tf);
  char buf[24];

  if (bt) {
    u8g2.drawXBMP(0, 0, BT_WIDTH, BT_HEIGHT, bluetooth_bmp);  
    u8g2.drawStr(12, 8, "BT Connected");
  } else {
    u8g2.drawStr(0, 8, "BT Disconnected");
  }

  snprintf(buf, sizeof(buf), "Speed: %d%%", spd);
  u8g2.drawStr(0, 20, buf);

  snprintf(buf, sizeof(buf), "Dir: %s", directions[dir]);
  u8g2.drawStr(0, 32, buf);

  snprintf(buf, sizeof(buf), "Runtime: %lus", time);
  u8g2.drawStr(0, 44, buf);

  u8g2.drawFrame(0, 54, 100, 8);  
  u8g2.drawBox(1, 55, map(spd, 0, 100, 0, 98), 6); 

  u8g2.sendBuffer();
}

// Safe EEPROM write with validation
void save_runtime_to_eeprom(uint32_t value) {
  uint32_t current_value;
  EEPROM.get(EEPROM_ADDR, current_value);
  
  // Only write if value changed significantly (save EEPROM writes)
  if (abs((long)(value - current_value)) > 1) { // Only save if difference > 1 minute
    EEPROM.put(EEPROM_ADDR, value);
  }
}

uint32_t load_runtime_from_eeprom() {
  uint32_t saved = 0;
  EEPROM.get(EEPROM_ADDR, saved);
  
  // Validate the read value
  if (saved == 0xFFFFFFFF || saved > 864000) { // Max ~10 days of runtime
    saved = 0;
    EEPROM.put(EEPROM_ADDR, saved);
  }
  
  return saved;
}

void setup() {
  Serial.begin(9600);
  oled_init();
  runtime = load_runtime_from_eeprom();
  Serial.print("Loaded runtime: ");
  Serial.println(runtime);
}

void loop() {
  unsigned long now = millis();
  
  // Update runtime every second
  if (now - last_tick >= 1000) {
    runtime++;
    last_tick = now;
    needs_save = true; // Mark that we need to save
  }
  
  // Save runtime to EEPROM periodically (with debouncing)
  if (needs_save && (now - last_save >= SAVE_INTERVAL)) {
    save_runtime_to_eeprom(runtime);
    last_save = now;
    needs_save = false;
    Serial.print("Saved runtime: ");
    Serial.println(runtime);
  }

  oled_draw_status(direction, speed, runtime, bt_connected);
  delay(300);
}