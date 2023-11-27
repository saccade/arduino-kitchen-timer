// author: kraemer <jkraemer@gmail.com>

// This code is part of a problem set, and will not work without modification.

#pragma once

/*
// Runs a 7 segment common cathode clock display with pinout (zero-indexed anodes):

0 A F 1 2 B
E D P C G 3

- Use ~100 ohm resistors or your display will fail at a bad time.

+ Resistors can go on segment or place side. (Segment side looks more even.)

- Place 8 anodes on sequential GPIOs; default low:

- Continue sequence with 4 cathode pins for places; default high.
*/

#include <stdint.h>

#define A_GPIO 6
#define B_GPIO 0
#define C_GPIO 4
#define D_GPIO 2
#define E_GPIO 7
#define F_GPIO 1
#define G_GPIO 3
#define P_GPIO 5

#define C0_GPIO 13
#define C1_GPIO 12
#define C2_GPIO 11
#define C3_GPIO 10

#define MS_PER_DIGIT 250

class SevenSegmentClock {
public:
  SevenSegmentClock();

  void refresh();

  void set_digit(uint32_t place, int value) {
    digits[place] = value;
    update_masks();
  }

  void set_point(int location) {
    point_location = location;
    update_masks();
  }

  // show leading digit only when non-zero:
  void set_first_pair(int count) {
    if (count < 0) {
      set_digit(0, -1);
      set_digit(1, -1);
      return;
    }
    int lead_digit = (count % 100) / 10;
    if (lead_digit == 0) {
      set_digit(0, -1);
    } else {
      set_digit(0, lead_digit);
    }
    set_digit(1, count % 10);
  }

  // always show leading digit:
  void set_second_pair(int count) {
    if (count < 0) {
      set_digit(0, -1);
      set_digit(1, -1);
      return;
    }
    set_digit(2, (count % 100) / 10);
    set_digit(3, count % 10);
  }

  // Call when display changes.
  void update_masks() {
    for (int i = 0; i < 4; i++) {
      masks[i] = place_masks[i];
      if (digits[i] >= 0) {
        masks[i] += digit_masks[digits[i]];
      }
      if (i == point_location) {
        masks[i] += SM_P;
      }
    }
  }

  void clear();

  void blink_hz(uint32_t frequency_hertz) {
    blink_frequency_hz = frequency_hertz;
    if (frequency_hertz) {
      blink_cycle_millis = 1000 / frequency_hertz;
      blink_half_millis = blink_cycle_millis / 2;
    } else {
      blink_cycle_millis = blink_half_millis = 0;
    }
    blink_phase_millis = millis() % blink_cycle_millis;
  }

  void blink_hz(uint32_t frequency_hertz, uint32_t duration_millis) {
    blink_end_system_millis = millis() + duration_millis;
    blink_hz(frequency_hertz);
  }

  void blink_stop() {
    blink_hz(0);
    blink_end_system_millis = 0;
  }

  int point_location = -1;

private:
  // Set up pin masks for segments and add them to draw the digits.

  // segment masks:
  const uint32_t SM_A =  1 << (A_GPIO);
  const uint32_t SM_B =  1 << (B_GPIO);
  const uint32_t SM_C =  1 << (C_GPIO);
  const uint32_t SM_D =  1 << (D_GPIO);
  const uint32_t SM_E =  1 << (E_GPIO);
  const uint32_t SM_F =  1 << (F_GPIO);
  const uint32_t SM_G =  1 << (G_GPIO);
  const uint32_t SM_P =  1 << (P_GPIO);

  /*
  segment names:
  |  AAAA
  | F    B
  | F    B
  |  GGGG
  | E    C
  | E    C
  |  DDDD   PP
  */

  // digit masks drawn in caligraphic order:
  const uint32_t DM_1 = SM_B + SM_C;
  const uint32_t DM_2 = SM_A + SM_B + SM_G + SM_E + SM_D;
  const uint32_t DM_3 = SM_A + SM_B + SM_C + SM_D + SM_G;
  const uint32_t DM_4 = SM_B + SM_C + SM_F + SM_G ;
  const uint32_t DM_5 = SM_A + SM_F + SM_G + SM_C + SM_D;
  const uint32_t DM_6 = SM_A + SM_F + SM_E + SM_D + SM_C + SM_G;
  const uint32_t DM_7 = SM_A + SM_B + SM_C;
  const uint32_t DM_8 = SM_A + SM_B + SM_C + SM_D + SM_E + SM_F + SM_G;
  const uint32_t DM_9 = SM_A + SM_B + SM_C + SM_D + SM_E + SM_F + SM_G;
  const uint32_t DM_0 = SM_A + SM_B + SM_C + SM_D + SM_E + SM_F + SM_G;

  uint32_t digit_masks[10] = {
    DM_0, DM_1, DM_2, DM_3, DM_4,
    DM_5, DM_6, DM_7, DM_8, DM_9
  };

  // all segments mask:
  const uint32_t SM_X = DM_8 + SM_P;

  // place masks:
  const uint32_t PM_0 =  1 << (C0_GPIO);
  const uint32_t PM_1 =  1 << (C1_GPIO);
  const uint32_t PM_2 =  1 << (C2_GPIO);
  const uint32_t PM_3 =  1 << (C3_GPIO);

  const uint32_t place_masks[4] = {PM_0, PM_1, PM_2, PM_3};

  // all places mask:
  const uint32_t PM_X = PM_0 + PM_1 + PM_2 + PM_3;

  // all pins mask:
  const uint32_t M_ALL = SM_X + PM_X;

  const uint32_t ms_per_cycle = 4 * MS_PER_DIGIT;

  // multiplex timing:
  uint32_t millis() ;
  uint32_t digit_to_show();

  // currently showing:
  uint32_t active_digit = 0;
  uint32_t active_mask = 0;

  int digits[4] = {-1, -1, -1, -1};
  uint32_t masks[4] = {PM_0, PM_1, PM_2, PM_3};

  // blink:
  uint32_t blink_frequency_hz = 0;
  uint32_t blink_cycle_millis = 0;
  uint32_t blink_half_millis = 0;
  uint32_t blink_end_system_millis = 0;
  uint32_t blink_phase_millis = 0;
  bool blink_active();
};
