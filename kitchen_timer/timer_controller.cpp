#include "timer_controller.h"

// toneAC uses pins 9 and 10.
#include "toneAC.h"

#include "Arduino.h"

TimerController::TimerController(int encoderPinA,
                                 int clockUpShift, int clockDownShift, float clockGain, float clockFade) {

  Serial.println("TimerController start");
  Serial.flush();

  rotary_encoder_ = new RotaryEncoder(encoderPinA);
  encoder_movement_ = rotary_encoder_->movement();
  button_is_pressed_ = false;


  clock_ = new TimerClock(clockUpShift, clockDownShift, clockGain, clockFade);
  clock_seconds_ = clock_->max_time_seconds_ + 1;

  // Set up 7-segment display.
  display_ = new Adafruit_7segment();
  display_->begin(0x70);
  display_->setBrightness(4);

  UpdateController();
}

void TimerController::UpdateController() {
  HandleClock();
  HandlePushButton();
  HandleRotaryEncoder();
  HandleOutput();
  HandleStopwatchReady();
  HandleSerialMessage();
  HandleTimedReset();
}

// Check for clock change; set update_clock_output_ if needed.
void TimerController::HandleClock() {
  if (clock_->IsActive()) {
    long time_seconds = clock_->Seconds();
    if (clock_seconds_ != time_seconds) {
      clock_seconds_ = time_seconds;
      update_clock_output_ = true;
    }
  }
}

// Reset timer or start/stop stopwatch when push-button is pressed.
void TimerController::HandlePushButton() {
  bool is_pressed = rotary_encoder_->is_pressed();
  if (is_pressed and not button_is_pressed_) {
    button_is_pressed_ = true;
    if (clock_->StopwatchIsStopped()) {
      ResetClock();
    } else if (clock_->TimerIsCountingDown()) {
      release_button_to_reset_ = true;
    } else if (clock_->StopwatchIsCountingUp()) {
      clock_->StopStopwatch();
    } else {
      clock_->ReadyStopwatch();
    }
    update_clock_output_ = true;
    HandleOutput();
  } else if (not is_pressed and button_is_pressed_) {
    button_is_pressed_ = false;
      if (release_button_to_reset_) {
        release_button_to_reset_ = false;
        ResetClock();
      } else if (clock_->StopwatchIsReady()) {
        clock_->StartStopwatch();
      }
      update_clock_output_ = true;
      HandleOutput();
  }
}

// Adjust clock if rotary encoder has moved.
void TimerController::HandleRotaryEncoder() {
  rotary_encoder_->update();
  if (rotary_encoder_->movement() < encoder_movement_) {
    if (not button_is_pressed_) {
      clock_->DecreaseTime();
      update_clock_output_ = true;
    } else {
      release_button_to_reset_ = false;
      clock_->SmallDecreaseTime();
      if (not clock_->IsActive()) {
        ResetClock();
      }
    }
  } else if (rotary_encoder_->movement() > encoder_movement_) {
    if (not button_is_pressed_) {
      clock_->IncreaseTime();
      update_clock_output_ = true;
    } else {
      release_button_to_reset_ = false;
      clock_->SmallIncreaseTime();
    }
  }
  encoder_movement_ = rotary_encoder_->movement();
}

// Update display if clock changed; flash and beep if time is up.
void TimerController::HandleOutput() {
  if (update_clock_output_) {

    // Send new display text if changed.
    String display_text = FormatTimeString(clock_seconds_);
    if (display_text_ != display_text) {
      Serial.println(FormatTimeString(clock_seconds_));
      display_text_ = display_text;
    }

    // Update display digits if changed.
    if (not clock_->IsActive()) {
      display_->clear();
      // Set out-of-range (but potentially displayable) dummy value.
      display_digits_ = FormatTimeDigits(clock_->max_time_seconds_ + 1);
    } else {
      int display_digits = FormatTimeDigits(clock_seconds_);
      if (display_digits_ != display_digits) {
        display_->print(abs(display_digits), DEC);
        if (abs(display_digits) < 100) {
          display_->writeDigitNum(1, 0, false);
          if (abs(display_digits) < 10) {
            display_->writeDigitNum(3, 0, false);
          }
        }

        // Flash display and beep for a while if time is up.
        if ((clock_seconds_ < 0) and (clock_seconds_ > -max_beep_sec_)) {
          if (display_digits % 3 == 1) {
            toneAC(toneHz, 10, toneMillis, true);
          } else if (display_digits % 3 == 2) {
            toneAC(toneHz * 4 / 5, 10, toneMillis * 6 / 5, true);
          }
          display_->blinkRate(HT16K33_BLINK_2HZ);
        } else {
          display_->blinkRate(HT16K33_BLINK_OFF);
        }
      }
      display_->drawColon(DisplayClockColon());
    }
    display_digits_ = FormatTimeDigits(clock_seconds_);
  }
  display_->writeDisplay();
  update_clock_output_ = false;
}

// Overrides colon when stopwatch is ready.
void TimerController::HandleStopwatchReady() {
  if (clock_->StopwatchIsReady()) {
    bool show_colon = (millis() % (1000 / stopwatch_flash_Hz_)) < (500 / stopwatch_flash_Hz_);
    if (show_colon and not stopwatch_ready_colon_showing_) {
      display_->drawColon(true);
      display_->writeDisplay();
      stopwatch_ready_colon_showing_ = true;
    } else if (not show_colon and stopwatch_ready_colon_showing_) {
      display_->drawColon(false);
      display_->writeDisplay();
      stopwatch_ready_colon_showing_ = false;
    }
  }
}

// Print debug info if there's serial input. Reset system if input begins with 'z'.
void TimerController::HandleSerialMessage() {
  if (Serial.available()) {
    String command = Serial.readString();
    if (command.length() and command[0] == 'z') {
      Serial.println("reset system.");
      ResetSystem();
    } else {
      Serial.println("encoder: " + String(rotary_encoder_->movement()));
      Serial.println("clock: " + String(clock_->seconds()));
      Serial.println("active: " + String(clock_->IsActive()));
    }
  }
}

// Reset CPU if system clock has reached max_millis_ and clock is not active.
void TimerController::HandleTimedReset() {
  if (millis() >= max_millis_ and not clock_->IsActive()) {
    Serial.println("timed reset");
    Serial.flush();
    TimerController::ResetSystem();
  }
}

void TimerController::ResetClock() {
  Serial.println("ResetClock");
  clock_->Reset();
  // Set value that won't occur, so change to 0 is apparent.
  clock_seconds_ = clock_->max_time_seconds_ + 1;
  update_clock_output_ = true;
  HandleOutput();
}

bool TimerController::DisplayClockColon() {
  if (IsHHMM(clock_seconds_) and not clock_->StopwatchIsStopped()) {
    return not bool(clock_seconds_ % 2);
  } else {
    return true;
  }
}

bool TimerController::IsHHMM(long seconds) {
  return (3600L <= abs(seconds)) and (abs(seconds) <= clock_->max_time_seconds_);  // 1 hour
}

// Format time in mm:ss or hh:mm as String.
static const String TimerController::FormatTimeString(long time_seconds) {
  bool negative = time_seconds < 0;
  if (negative) {
    time_seconds *= -1;
  }
  int hours = time_seconds / 3600;
  int remaining_time_seconds = time_seconds % 3600;
  int minutes = remaining_time_seconds / 60;
  int seconds = remaining_time_seconds % 60;
  if (hours == 1 and minutes < 40) {  // < 100 min fits in mm:ss.
    hours = 0;
    minutes += 60;
  }
  char buffer[8];
  if (negative) {
    if (hours) {
      sprintf(buffer, "-%02d:%02d *", hours, minutes);
    } else {
      sprintf(buffer, "-%02d:%02d", minutes, seconds);
    }
  } else {
    if (hours) {
      sprintf(buffer, "%02d:%02d *", hours, minutes);
    } else {
      sprintf(buffer, "%02d:%02d", minutes, seconds);
    }
  }

  return String(buffer);
}

// Format time in mm:ss or hh:mm as a base-10 integer.
static const int TimerController::FormatTimeDigits(long time_seconds_in) {
  long time_seconds = abs(time_seconds_in);
  int hours, minutes, seconds = 0;
  hours = time_seconds / 3600;
  int remaining_time_seconds = time_seconds % 3600;
  minutes = remaining_time_seconds / 60;
  seconds = remaining_time_seconds % 60;
  if (hours) {
    return hours * 100 + minutes;
  } else {
    return minutes * 100 + seconds;
  }
}
