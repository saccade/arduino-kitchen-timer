#pragma once

#include "timer_clock.h"
#include "rotary_encoder.h"

#include "Adafruit_LEDBackpack.h"

// Constructs and manages peripheral interface and timer objects.
class TimerController {
public:
  TimerController(
    int encoderPinA,
    int clockUpShift, int clockDownShift, float clockGain, float clockFade);

  // Must by called by client in loop().
  void UpdateController();

private:
  void HandleClock();
  void HandleRotaryEncoder();
  void HandlePushButton();
  void HandleOutput();
  void HandleStopwatchReady();
  void HandleBrightness();
  void HandleSerialMessage();
  void HandleTimedReset();
  void HandleSleep();

  static void WakeUp();

  // Also sets clock_seconds to dummy value.
  void ResetClock();

  bool DisplayClockColon();

  // Component interfaces.
  TimerClock* clock_;
  RotaryEncoder* rotary_encoder_;
  Adafruit_7segment* display_;

  // Store values to check for changes.
  long encoder_movement_ = 0;
  long clock_seconds_ = 0;
  bool button_is_pressed_ = false;
  String display_text_ = "";
  int display_digits_ = 8888;
  bool colon_displayed_ = true;
  bool release_button_to_reset_ = false;

  // Reset every 2^30 ms (12.4 days).
  const unsigned long max_millis_ = 1073741824;
  void (*ResetSystem)(void);  // Segfault to reset (reset pin causes upload problems).

  // Speaker settings.
  const int toneHz = 800;
  const int toneMillis = 150;
  const int max_beep_sec_ = 15 * 60;  // 15 minutes

  // Set true if clock seconds changes.
  bool update_clock_output_ = false;

  int stopwatch_flash_Hz_ = 2;
  bool stopwatch_ready_colon_showing_ = false;

  // Used in display formatting (not now) and colon flashing logic.
  bool IsHHMM(long seconds);

  // mm:ss / hh:mm formatting for serial output and clock display.
  static const String FormatTimeString(long seconds);
  static const int FormatTimeDigits(long seconds);
};