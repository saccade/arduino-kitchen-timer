// Use analog pin A0 to check LED supply voltage.
#define ANALOG_READ_PIN A0

float led_voltage() {
  int sensor = analogRead(ANALOG_READ_PIN);  // sensor reading is between 0 and 1024
  float voltage = 5.0 * sensor / 1024;       // voltage is between 0 and 5 V.
  return voltage;
}

// Check LED supply voltage.
void check_led_voltage(int led_write_pin) {
  digitalWrite(led_write_pin, HIGH);  // turn on
  float voltage = led_voltage();
  Serial.print("LED voltage: " + String(voltage));
  if (voltage < 1.9) {
    Serial.println("\nSupply voltage is too low; LED may not light. (Decrease limiting resistor?)");
    Serial.print("Waiting five seconds...   ");
    delay(5000);
    Serial.println("Proceeding.");
  } else if (voltage > 3.0) {
    digitalWrite(led_write_pin, LOW);  // turn off
    Serial.println("\nSupply voltage is too high! Stopping.");
    delay(100);
    exit(0);
  } else {
    Serial.println(" (ok)");
  }
}
