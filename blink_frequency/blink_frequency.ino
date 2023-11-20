#include "check_voltage.h"

// Use pin 7 for LED.
#define LED_WRITE_PIN 7

// These variables get declared here so the loop can use them.

// Initially sleep for 1000 ms (1 second).
int base_sleep_millis = 1000;

// Keep track of how many times loop is run.
int step_count = 0;

// initial LED logic state.
bool led_state = true;

// setup() runs once at startup, then loop() starts.
void setup() {
  Serial.begin(115200);          // start modem.
  delay(100);                    // wait for modem and serial monitor to sync up.
  Serial.println("\nstart");     // here we go!
  pinMode(LED_WRITE_PIN, OUTPUT);  // ready LED

  check_led_voltage(LED_WRITE_PIN);
}

// loop() runs over and over until something stops it (power loss, software crash, thermal breakdown...).
void loop() {
  // Update loop counter.
  step_count = step_count + 1;

  
  // Switch logic state.
  led_state = not led_state;
  
  // Switch the on-board LED on or off according to logic state.
  digitalWrite(LED_WRITE_PIN, led_state? HIGH: LOW);
  

  // This is how long we'll sleep before switching the LED again.
  // The time between gets shorter and shorter; you can safely ignore the math.
  int millis_to_sleep = round(base_sleep_millis / pow(step_count, 0.75));

  // Print some info to the serial modem.
  Serial.print("step " + String(step_count));
  Serial.print("; LED: " + String(led_state));
  Serial.print("; sleep millis: " + String(millis_to_sleep));
  Serial.println("; LED frequency: " + String(round(1000 / millis_to_sleep)) + " Hz");

  // Go to sleep for now.
  delay(millis_to_sleep);
}
