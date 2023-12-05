#include "timer_clock.h"

#include "Arduino.h"
#include "math.h"

TimerClock::TimerClock(int up_shift, int down_shift, float gain, float fade) {
  up_shift_ = up_shift;
  down_shift_ = down_shift;
  up_gain_ = gain;
  fade_ = fade;

  down_gain_ = pow(gain, (-1 / (fade + 1)));
  small_up_gain_ = pow(up_gain_, 1.0 / small_change_factor_);
  small_down_gain_ = pow(down_gain_, 1.0 / small_change_factor_);
}

void TimerClock::UpdateClock() {
  if (end_millis_) {
    long millis_remaining = end_millis_ - millis();
    if (ceil(millis_remaining / 1000.0) != seconds_) {
      seconds_ = ceil(millis_remaining / 1000.0);
    }
  }
  if (start_millis_) {
    long seconds_elapsed = floor((millis() - start_millis_) / 1000.0);
    if (seconds_elapsed != seconds_) {
      seconds_ = seconds_elapsed;
    }
  }
}

bool TimerClock::TimerIsCountingDown() const {
  return end_millis_;
}

bool TimerClock::StopwatchIsReady() const {
  return stopwatch_ready_;
}

bool TimerClock::StopwatchIsCountingUp() const {
  return start_millis_;
}

bool TimerClock::StopwatchIsStopped() const {
  return stopwatch_stopped_;
}

bool TimerClock::IsActive() const {
  return TimerIsCountingDown() or StopwatchIsReady() or StopwatchIsCountingUp() or StopwatchIsStopped();
}

void TimerClock::SetCountdownTimer(long time_seconds) {
  if (time_seconds <= 0) {
    Reset();
    return;
  }
  time_seconds = min(time_seconds, max_time_seconds_);
  end_millis_ = millis() + 1000 * time_seconds;
  UpdateClock();
}

void TimerClock::AdjustTime(int shift, float gain) {
  UpdateClock();
  if (end_millis_ == 0 and shift < 1) { return; }
  if (seconds_ < 0) { return; }        // Time is up; can't adjust.
  if (start_millis_) { return; }       // Can't adjust stopwatch.
  if (stopwatch_ready_) { return; }    // ^^^
  if (stopwatch_stopped_) { return; }  // ^^^
  if (shift == 0) {
    seconds_ = round(seconds_ * gain);
  } else if (shift > 0) {
    seconds_ = max(RoundBy(seconds_ + shift, shift), RoundBy((seconds_ * gain), shift));
  } else {  // shift < 0
    seconds_ = min(RoundBy(seconds_ + shift, shift), RoundBy((seconds_ * gain), shift));
  }
  SetCountdownTimer(seconds_);
}

void TimerClock::IncreaseTime() {
  AdjustTime(up_shift_, up_gain_);
}

void TimerClock::DecreaseTime() {
  AdjustTime(down_shift_, down_gain_);
}

void TimerClock::SmallIncreaseTime() {
  AdjustTime(-down_shift_, small_up_gain_);  // Use down_shift for smaller increases.
}


void TimerClock::SmallDecreaseTime() {
  AdjustTime(down_shift_ / 2, small_down_gain_);
}

void TimerClock::ReadyStopwatch() {
  seconds_ = 0;
  stopwatch_ready_ = true;
}

void TimerClock::StartStopwatch() {
  start_millis_ = millis();
  stopwatch_ready_ = false;
}

// Time on clock will no longer advance.
void TimerClock::StopStopwatch() {
  start_millis_ = 0;
  stopwatch_stopped_ = true;
}

long TimerClock::Seconds() {
  UpdateClock();
  return seconds_;
}

long TimerClock::seconds() const {
  return seconds_;
}

void TimerClock::Reset() {
  if (not IsActive()) { return; }
  end_millis_ = 0;
  start_millis_ = 0;
  seconds_ = 0;
  stopwatch_ready_ = false;
  stopwatch_stopped_ = false;
}

const long TimerClock::RoundBy(float value, int increment) {
  return round(value / increment) * increment;
}
