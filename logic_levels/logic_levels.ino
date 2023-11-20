#include "logging.h"

#define DIGITAL_READ_PIN 3
#define LED_WRITE_PIN 7
#define ANALOG_READ_PIN A0

LogEveryNSec logger;

void setup() {
  Serial.begin(115200);            // start modem.
  delay(100);                      // wait for modem and serial monitor to sync up.
  Serial.println("\nstart");       // here we go!
  pinMode(LED_WRITE_PIN, OUTPUT);  // ready LED
  digitalWrite(LED_WRITE_PIN, true);
  logger = LogEveryNSec(1);
}

void loop() {
  logger.Log("a: " + String(analogRead(ANALOG_READ_PIN)) + "; d: " + String(digitalRead(DIGITAL_READ_PIN)));
}
