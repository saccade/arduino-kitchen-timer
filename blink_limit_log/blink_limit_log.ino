#define LED_WRITE_PIN 7

#include "led_voltage.h"
#include "logging.h"

LogEveryNSec logger;

void setup() {
  Serial.begin(115200);          // start modem.
  delay(100);                    // wait for modem and serial monitor to sync up.
  Serial.println("\nstart");     // here we go!

  // ready LED
  pinMode(LED_WRITE_PIN, OUTPUT);
  check_led_voltage(LED_WRITE_PIN);

  // logger = LogEveryNSec(3.812);  // log at pi/12 Hz
  logger = LogEveryNSec(0.5);  // log at 4 Hz
}

long base_sleep_micros = 1e6;
long count = 0;

bool led_state = false;

// Flips the physical LED and returns the next state.
bool flip_led(bool state) {
  digitalWrite(LED_WRITE_PIN, not state);
  return not state;
}

// Wait for the next cycle.
void wait(long micros_to_sleep) { 
  // delayMicroseconds is for short delays, and can only wait ~1/15 second.
  if (micros_to_sleep < 65536) {   // 2^16 micros max
    delayMicroseconds(micros_to_sleep);
  } else {
    delay(micros_to_sleep / 1000);
  }
}

void loop() {
  count++;
  led_state = flip_led(led_state);

  long micros_to_sleep = round(base_sleep_micros / pow(count, 0.6));
  logger.Log("step " + String(count) + "; sleep micros: " + String(micros_to_sleep) + "; LED frequency: " + String(round(1e6 / micros_to_sleep)) + " Hz; LED frequency: " + String(round(1e6 / micros_to_sleep)) + " Hz; LED voltage: " + String(led_voltage()) + " V");
  wait(micros_to_sleep);
}
