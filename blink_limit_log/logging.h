#pragma once

#include "WString.h"

class LogEveryNSec {
public:
  LogEveryNSec(float n_seconds);
  LogEveryNSec();
  
  void Log(String log_line);

  private:
  long int micros_between = 1e6;
  long next_log_micros = 0;
};