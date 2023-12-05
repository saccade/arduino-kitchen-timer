#include "HardwareSerial.h"
#include "rotary_encoder.h"

#include <Arduino.h>

RotaryEncoder::RotaryEncoder(int pinA) {
  pinA_ = pinA;
  pinB_ = pinA + 1;
  pinC_ = pinA + 2;
  pinD_ = pinA + 3;
  pinE_ = pinA + 4;

  rotationState_ = UNKNOWN;
  movement_ = 0;

  pinMode(pinA_, INPUT_PULLUP);
  pinMode(pinB_, INPUT_PULLUP);
  pinMode(pinC_, INPUT_PULLUP);

  pinMode(pinD_, OUTPUT);
  digitalWrite(pinD_, HIGH);

  pinMode(pinE_, OUTPUT);
  digitalWrite(pinE_, LOW);

  update();
}

void RotaryEncoder::zero() {
  movement_ = 0;
  rotationState_ = UNKNOWN;
  update();
}

void RotaryEncoder::update() {
  bool stateA = digitalRead(pinA_);
  bool stateB = digitalRead(pinB_);

  // first detent (1, 1)
  if (stateA and stateB) {
    if (rotationState_ == LEFT_3) {
      movement_--;
    } else if (rotationState_ == RIGHT_3) {
      movement_++;
    }
    rotationState_ = NEUTRAL;
  }

  // starting left / ending right (1, 0)
  else if (stateA and not stateB) {
    if (rotationState_ == LEFT_1 || rotationState_ == RIGHT_3) {  // nothing
    } else if (rotationState_ == NEUTRAL || rotationState_ == LEFT_2) {
      rotationState_ = LEFT_1;
    } else if (rotationState_ == RIGHT_2) {
      rotationState_ = RIGHT_3;
    } else {
      rotationState_ = UNKNOWN;
    }
  }

  // starting right / ending left (0, 1)
  else if (not stateA and stateB) {
    if (rotationState_ == RIGHT_1 || rotationState_ == LEFT_3) {  // nothing
    } else if (rotationState_ == NEUTRAL || rotationState_ == RIGHT_2) {
      rotationState_ = RIGHT_1;
    } else if (rotationState_ == LEFT_2) {
      rotationState_ = LEFT_3;
    } else {
      rotationState_ = UNKNOWN;
    }
  }

  // second detent (0, 0)
  else {
    if (rotationState_ == LEFT_2 || rotationState_ == RIGHT_2) {  // nothing
    } else if (rotationState_ == LEFT_1) {
      movement_--;
      rotationState_ = LEFT_2;
    } else if (rotationState_ == LEFT_3) {
      movement_++;
      rotationState_ = LEFT_2;
    } else if (rotationState_ == RIGHT_1) {
      movement_++;
      rotationState_ = RIGHT_2;
    } else if (rotationState_ == RIGHT_3) {
      movement_--;
      rotationState_ = RIGHT_2;
    } else {
      rotationState_ = LEFT_2;
    }
  }
}

bool RotaryEncoder::is_pressed() const {
  return not digitalRead(pinC_);
}

int RotaryEncoder::rotation_state() {
  return rotationState_;
}
