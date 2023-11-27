#include "seven_segment.h"

SevenSegmentClock display;

uint32_t ms_per_frame = 2000;
uint32_t frame = 0;

void update_digits(SevenSegmentClock& display) {
  if (millis() / ms_per_frame <= frame) {
    return;
  }
  frame = (millis() / ms_per_frame) % 10;
  display.set_first_pair(11 * frame + 01);
  display.set_second_pair(11 * frame + 23);
}

void setup() {

}

void loop() {
  update_digits(display);
  display.refresh();
}
