
// Use digital pin 7 for LED.
#define LED_WRITE_PIN 7

// Use analog pin A0 to check LED supply voltage.
#define ANALOG_READ_PIN A0

// These variables get declared here so that loop() can use them as storage.

// Sleep for 1000 ms (1 second).
const int millis_to_sleep = 1000;

// Keep track of how many times loop is run.
int step_count = 0;

// initial LED logic state.
bool led_state = true;

// setup() runs once at startup, then loop() starts.
void setup() {
  Serial.begin(115200);                    // start modem.
  delay(100);                              // wait for modem and serial monitor to sync up.
  Serial.println("\nstart");               // here we go!
  pinMode(LED_WRITE_PIN, OUTPUT);          // ready LED
  pinMode(ANALOG_READ_PIN, INPUT_PULLUP);  // pullup defaults to HIGH

  // Check LED supply voltage.
  digitalWrite(LED_WRITE_PIN, HIGH);         // turn on
  int sensor = analogRead(ANALOG_READ_PIN);  // sensor reading is between 0 and 1024
  float voltage = 5.0 * sensor / 1024;       // voltage is between 0 and 5 V.
  Serial.print("LED voltage: " + String(voltage));
  if (voltage < 1.9) {
    Serial.println("\nSupply voltage is too low; LED may not light. (Decrease limiting resistor?)");
    Serial.print("Waiting five seconds...   ");
    delay(5000);
    Serial.println("Proceeding.");
  } else if (voltage > 3.0) {
    digitalWrite(LED_WRITE_PIN, LOW);  // turn off
    Serial.println("\nSupply voltage is too high! Stopping.");
    delay(100);
    exit(0);
  } else {
    Serial.println(" (ok)");
  }
}

// loop() runs over and over until something stops it (power loss, software crash, thermal breakdown...).
void loop() {
  // Update loop counter.
  step_count = step_count + 1;

  // Switch logic state.
  led_state = not led_state;
  // Switch the on-board LED on or off according to logic state.
  if (led_state) {
    digitalWrite(LED_WRITE_PIN, HIGH);
  } else {
    digitalWrite(LED_WRITE_PIN, LOW);
  }

  // Print some info to the serial modem.
  Serial.print("step " + String(step_count));
  Serial.print("; LED: " + String(led_state));
  Serial.print("; sleep millis: " + String(millis_to_sleep));
  Serial.println("; LED frequency: " + String(round(1000 / millis_to_sleep)) + " Hz");

  // Go to sleep for now.
  delay(millis_to_sleep);
}
