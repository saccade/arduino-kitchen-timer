#include "timer_controller.h"

const int rotaryEncoderPinA = 2;  // pin for CLK; others assigned sequentially

const int clockUpShift = 30;     // seconds per increase.
const int clockDownShift = -10;  // seconds per decrease.
const float clockGain = 1.1;     // >= 1.0 to increase faster.
const float clockFade = 1.24;    // >0 to decrease slower than increase.

TimerController* timer_controller;

void setup() {
  Serial.begin(115200);

  timer_controller = new TimerController(
    rotaryEncoderPinA,
    clockUpShift, clockDownShift, clockGain, clockFade);
}

void loop() {
  timer_controller->UpdateController();
}
