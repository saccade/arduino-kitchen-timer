#include "rotary_encoder.h"

#define DEBUG_ENCODER_STATE false

const int pinA = 2;
const int pinB = 3;
const int pinC = 4;

int movement = 0;
int rotation_state = 37;

bool pressed = false;

RotaryEncoder* encoder;

void setup() {
  Serial.begin(115200);
  delay(100);
  Serial.println("start");

  encoder = new RotaryEncoder(pinA, pinB, pinC);
  pinMode(5, OUTPUT);
  digitalWrite(5, HIGH);
  pinMode(6, OUTPUT);
  digitalWrite(6, LOW);
}


void loop() {
  encoder->update();
  if (DEBUG_ENCODER_STATE) {
    if (rotation_state != encoder->rotation_state()) {
      rotation_state = encoder->rotation_state();
      Serial.println("s: " + String(rotation_state));
    }
  }

  if (movement != encoder->movement()) {
    movement = encoder->movement();
    Serial.println("m: " + String(movement));
  }

  if (encoder->is_pressed() != pressed) {
    if (encoder->is_pressed()) {
      PrintAndZero();
    }
    pressed = encoder->is_pressed();
  }

  // If there's serial input, print the encoder movement. Zero the
  // encoder if the input begins with 'z'.
  if (Serial.available()) {
    String command = Serial.readString();
    if (command.length() && command[0] == 'z') {
      PrintAndZero();
    } else
      Serial.println("m: " + command + " " + String(encoder->movement()));
  }
}

// Print movement and zero encoder.
void PrintAndZero() {
  if (not encoder->movement()) { return; }

  Serial.println("m: " + String(encoder->movement()) + "; zeroing.");
  encoder->zero();
}
