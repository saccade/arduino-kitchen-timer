// See header for documentation.

#include "seven_segment.h"

#include "hardware/gpio.h"
#include "pico/stdlib.h"

SevenSegmentClock::SevenSegmentClock() {
  // set output pins:
  gpio_init_mask(M_ALL);
  gpio_set_dir_out_masked(M_ALL);

  // invert cathode signals:
  gpio_set_outover(C0_GPIO, GPIO_OVERRIDE_INVERT);
  gpio_set_outover(C1_GPIO, GPIO_OVERRIDE_INVERT);
  gpio_set_outover(C2_GPIO, GPIO_OVERRIDE_INVERT);
  gpio_set_outover(C3_GPIO, GPIO_OVERRIDE_INVERT);
}

uint32_t SevenSegmentClock:: millis() {
  return to_ms_since_boot(get_absolute_time());
}

uint32_t SevenSegmentClock::digit_to_show() {
  return (millis() % ms_per_cycle) / MS_PER_DIGIT;
}

void SevenSegmentClock::clear() {
  gpio_clr_mask(active_mask);
  blink_stop();
  for (int i = 0; i < 4; i++) {
    digits[i] = -1;
    masks[i] = place_masks[i];
  }
  refresh();
}

void SevenSegmentClock::refresh() {
  uint32_t digit = digit_to_show();
  if (digit == active_digit) return;

  // time to update:
  gpio_clr_mask(active_mask);
  active_digit = digit;
  if (blink_frequency_hz and blink_active()) {
    active_mask = place_masks[active_digit];
  } else {
    active_mask = masks[active_digit];
  }
  gpio_set_mask(active_mask);
}

// blanks screen when true; also handles blink duration.
bool SevenSegmentClock::blink_active() {
  uint32_t system_millis = millis();
  if (blink_end_system_millis and system_millis >= blink_end_system_millis) {
    blink_hz(0);
    return false;
  }
  uint32_t phase_adjusted_millis = system_millis - blink_phase_millis;
  return phase_adjusted_millis % blink_cycle_millis < blink_half_millis;
}
