#include "logging.h"

#include "Arduino.h"
#include "HardwareSerial.h"

LogEveryNSec::LogEveryNSec(float n_seconds) : 
  micros_between(max(0, round(n_seconds * 1e6))) { }

LogEveryNSec::LogEveryNSec() = default;

void LogEveryNSec::Log(String log_line) {
  if (micros() >= next_log_micros) {
    Serial.println(log_line);
    next_log_micros = micros() + micros_between;
  }
}