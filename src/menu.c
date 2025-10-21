#include "menu.h"
//libraries for the OLED screen



//Configuration for the OLED display





//Button configuration
#define BTN_NEXT   2
#define BTN_PREV   3
#define BTN_SELECT 4
#define DEBOUNCE_MS 150

//Menu structure
enum MenuItem {
    MENU_MODE,
    MENU_CONNECTION,
    MENU_TIME,
    MENU_COUNT
};

static int current_index = 0;
static int selected_index = -1;
static unsigned long last_button_time = 0;

//Mode selection 
static const char *mode_names[] = { "Slave", "Autonomous", "Control" };
static int current_mode = 0;

//System time
static unsigned long start_time = 0;

//Display abstraction, need to connect to the OLED later
void display_init();
void display_clear();
void display_print_line(int line, const char *text, bool highlight);
void display_show_selected(const char *item);
void display_refresh();

//Internal helpers
static void handle_buttons();
static void draw_menu();
static void update_menu_data();
static void format_time(char *buffer, unsigned long ms);

//Initialization
void menu_init() {
    pinMode(BTN_NEXT, INPUT_PULLUP);
    pinMode(BTN_PREV, INPUT_PULLUP);
    pinMode(BTN_SELECT, INPUT_PULLUP);

    start_time = millis();

    display_init();
    draw_menu();
}

//Main update function
void menu_update() {
    handle_buttons();
    update_menu_data();
}

//Button handling
static void handle_buttons() {
    unsigned long now = millis();
    if (now - last_button_time < DEBOUNCE_MS) return;

    if (digitalRead(BTN_NEXT) == LOW) {
        current_index = (current_index + 1) % MENU_COUNT;
        draw_menu();
        last_button_time = now;
    }

    if (digitalRead(BTN_PREV) == LOW) {
        current_index = (current_index - 1 + MENU_COUNT) % MENU_COUNT;
        draw_menu();
        last_button_time = now;
    }

    if (digitalRead(BTN_SELECT) == LOW) {
        //Handle selection action
        if (current_index == MENU_MODE) {
            current_mode = (current_mode + 1) % 3;
        } else if (current_index == MENU_CONNECTION) {
            //link this to the Bluetooth logic
        }
        draw_menu();
        last_button_time = now;
    }
}

//Menu content update 
static void update_menu_data() {
    static unsigned long last_update = 0;
    unsigned long now = millis();
    if (now - last_update > 1000) {
        draw_menu();
        last_update = now;
    }
}

//Draw menu 
static void draw_menu() {
    display_clear();

    for (int i = 0; i < MENU_COUNT; i++) {
        bool highlight = (i == current_index);
        char line_text[32];

        switch (i) {
            case MENU_MODE:
                snprintf(line_text, sizeof(line_text), "Mode: %s", mode_names[current_mode]);
                break;
            case MENU_CONNECTION:
                snprintf(line_text, sizeof(line_text), "Connection: Bluetooth");
                break;
            case MENU_TIME: {
                char time_str[16];
                format_time(time_str, millis() - start_time);
                snprintf(line_text, sizeof(line_text), "System Time: %s", time_str);
                break;
            }
            default:
                snprintf(line_text, sizeof(line_text), "Unknown");
                break;
        }

        display_print_line(i, line_text, highlight);
    }

    display_refresh();
}

//Format time as mm:ss 
static void format_time(char *buffer, unsigned long ms) {
    unsigned long seconds = ms / 1000;
    unsigned long minutes = seconds / 60;
    seconds = seconds % 60;
    snprintf(buffer, 16, "%02lu:%02lu", minutes, seconds);
}

//External getters
int menu_get_selected() {
    return selected_index;
}
void menu_redraw() {
    draw_menu();
}
int menu_get_mode() {
    return current_mode;
}
