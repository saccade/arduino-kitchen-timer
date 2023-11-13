
void setup() {
  Serial.begin(115200);
  Serial.println("ok");
  pinMode(LED_BUILTIN, OUTPUT);
}

int base_sleep_millis = 2000;
int count = 0;

bool led_state = false;

void loop() {
  led_state = not led_state;
  Serial.println("step + " + String(count) + "; LED: " + String(led_state));
  if (led_state) {
    digitalWrite(LED_BUILTIN, HIGH);
  } else {
    digitalWrite(LED_BUILTIN, LOW);
  }

  int millis_to_sleep = base_sleep_millis / count;
  Serial.println("sleeping for " + String(millis_to_sleep) + " ms");
  delay(millis_to_sleep);
}
