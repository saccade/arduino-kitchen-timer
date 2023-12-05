#pragma once

// Timer: log(seconds) ~ timer increase count.
class TimerClock {
public:
  // Counts down seconds. Increase: time max(*gain, +-shift). Decrease dampened by fade.
  TimerClock(int up_shift, int down_shift, float gain, float fade);

  // Track millis; UpdateClock or Seconds() must be called by client in loop().
  void UpdateClock();

  bool TimerIsCountingDown() const;
  bool StopwatchIsReady() const;
  bool StopwatchIsCountingUp() const;
  bool StopwatchIsStopped() const;

  // True if TimerIsCountingDown or StopwatchIs[...]
  bool IsActive() const;

  // Set new time.
  void SetCountdownTimer(long time_seconds);

  // Multiplicatively ("log-scale") adjusts time remaining by up_gain_;
  // always adjust by at least shift (in direction indicated.) Does
  // nothing if time remaining is negative.
  void AdjustTime(int shift, float gain);

  // Adjust time up using up_shift_ and up_gain_.
  void IncreaseTime();

  // Adjust time down using down_shift_ and down_gain.
  void DecreaseTime();

  // Adjust time up using up_shift_ and small_up_gain_.
  void SmallIncreaseTime();

  // Adjust time down by down_shift_.
  void SmallDecreaseTime();

  // Set display seconds to zero and 'ready' flag to flash colon.
  void ReadyStopwatch();

  // Start counting seconds up.
  void StartStopwatch();

  // Stop advancing time on stopwatch and await reset.
  void StopStopwatch();

  // Update and return seconds remaining.
  long Seconds();

  // Seconds remaining.
  long seconds() const;

  // Deactivate timer.
  void Reset();

  // 1 day maximum timer duration in seconds.
  const long max_time_seconds_ = 24L * 60 * 60;

private:
  // Value of millis() when count-down timer reaches zero.
  unsigned long end_millis_ = 0;

  // Value of millis() when count-up timer was started.
  unsigned long start_millis_ = 0;

  bool stopwatch_ready_ = false;

  bool stopwatch_stopped_ = false;

  // Seconds clock wants to report; updated using end_millis_ or start_millis.
  long seconds_ = 0;

  // Seconds per increase.
  int up_shift_;

  // Seconds per decrease.
  int down_shift_;

  // Multiplicative up_gain_ for timer increase.
  float up_gain_;

  // (1 + fade_) decreases is comparable to an increase.
  float fade_;

  int small_change_factor_ = 8;

  // Store repeatedly-used values.
  float down_gain_;
  float small_up_gain_;
  float small_down_gain_;

  static const long RoundBy(float value, int increment);
};