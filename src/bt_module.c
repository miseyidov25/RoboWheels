#include <SoftwareSerial.h>
SoftwareSerial hc(2, 3); // RX, TX


// ─── Motor control functions ──────────────────────────────────────────────
void motors_forward() {
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void motors_reverse() {
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void motors_left() {
  // Left wheel backward, right wheel forward
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, LOW);
}

void motors_right() {
  // Left wheel forward, right wheel backward
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, HIGH);
}

void motors_brake() {
  // Both wheels shorted = hard stop
  digitalWrite(IN1, HIGH);
  digitalWrite(IN2, HIGH);
  digitalWrite(IN3, HIGH);
  digitalWrite(IN4, HIGH);
}

void motors_coast() {
  // Both wheels floating = soft stop
  digitalWrite(IN1, LOW);
  digitalWrite(IN2, LOW);
  digitalWrite(IN3, LOW);
  digitalWrite(IN4, LOW);
}

// ─── Setup & Loop ─────────────────────────────────────────────────────────
void setup() {
  // PC & HC-06 at 9600 baud
  Serial.begin(9600);
  BT.begin(9600);
  Serial.println("Bluetooth Robot Ready");

  // Configure motor pins
  pinMode(IN1, OUTPUT);
  pinMode(IN2, OUTPUT);
  pinMode(IN3, OUTPUT);
  pinMode(IN4, OUTPUT);
  pinMode(ENA, OUTPUT);

  // Full speed on ENA and start coasting
  analogWrite(ENA, 255);
  motors_coast();
}

void loop() {
  // Forward anything typed in Serial Monitor to HC-06
  if (Serial.available()) {
    BT.write(Serial.read());
  }

  // Read one command byte from HC-06
  if (BT.available()) {
    char cmd = BT.read();
    Serial.print("Got: ");
    Serial.println(cmd);

    // Dispatch to your motor functions
    switch (cmd) {
      case 'F': motors_forward();   break;
      case 'B': motors_reverse();   break;
      case 'L': motors_left();      break;
      case 'R': motors_right();     break;
      case 'S': motors_brake();     break;
      case 'C': motors_coast();     break;
      default:  /* ignore */        break;
    }
  }
}
