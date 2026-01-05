#include "line.h"
#include "pins.h"
#include "motors.h"
#include <avr/io.h>
#include <stdbool.h>

#define HIGH 1
#define LOW 0

void uart_println(const char* str);

void line_update() {
  
  bool left   = !(PIND & (1 << PD4));
  bool middle = !(PIND & (1 << PD3));
  bool right  = !(PIND & (1 << PD2));

  // Line-following decisions
  // Compare the sampled boolean values to LOW/HIGH (do NOT call digitalRead(LOW))
  //LOW = on line, HIGH = off line (i think)

  if (left == HIGH && middle == LOW && right == HIGH) {
    uart_println("Forward");
    motors_forward();
  } else if (left == HIGH && middle == HIGH && right == HIGH) {
    uart_println("All high");
    motors_right();
  } else if (left == HIGH && middle == LOW && right == LOW) {
    uart_println("Right & middle");
    motors_forward();
  }else if (left == HIGH && middle == HIGH && right == LOW) {
    uart_println("Right");
    motors_right();
  } else if (left == LOW && middle == LOW && right == HIGH) {
    uart_println("Left & middle");
    motors_forward();
  }else if (left == LOW && middle == HIGH && right == HIGH) {
    uart_println("Left");
    motors_left();
  } else if (left == LOW && middle == LOW && right == LOW) {
    uart_println("All low");
    motors_right();
  }else {
    uart_println("Something wrong");
    motors_coast();
  }
}
