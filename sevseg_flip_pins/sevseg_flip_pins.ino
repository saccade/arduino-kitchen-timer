#include <map>

std::map<int, bool> pins = {
  { 0, false },
  { 1, false },
  { 2, false },
  { 3, false },
  { 4, false },
  { 5, false },
  { 6, false },
  { 7, false },
  { 10, false },
  { 11, false },
  { 12, false },
  { 13, false },
};

void prompt() {
  Serial.println("enter pincode (0-7 for anodes; A-D for cathodes; P to print):");
}

int read_serial() {
  if (not Serial.available()) return -1;

  String input = Serial.readString();
  if (not input.length()) return -1;

  char pincode = input[0];
  if ('0' <= pincode and pincode <= '7') {
    return pincode - '0';
  } else if ('A' <= pincode and pincode <= 'D') {
    Serial.println("A");
    return pincode - 'A' + 10;
  } else if ('a' <= pincode and pincode <= 'd') {
    return pincode - 'a' + 10;
    Serial.println("a");
  } else if ('p' == pincode or 'P' == pincode) {
    print_pins();
    return -1;
  } else {
    Serial.print("couldn't read input " + input);
    prompt();
    return -1;
  }
}

void flip_pin(int pin) {
  bool state = not pins[pin];
  pins[pin] = state;
  Serial.println("pin " + String(pin) + ": " + String(state ? "HIGH" : "LOW"));
  digitalWrite(pin, state);
}

void print_pins() {
  Serial.print("LOW: ");
  for (auto& [pin, val] : pins) {
    if (not val) {
      Serial.print(pin);
      Serial.print(" ");
    }
  }
  Serial.println();

  Serial.print("HIGH: ");
  for (auto& [pin, val] : pins) {
    if (val) {
      Serial.print(pin);
      Serial.print(" ");
    }
  }
  Serial.println();
}

void setup() {
  Serial.begin(115200);
  Serial.println("start");  // modem won't catch this.
  gpio_init_mask(15615);
  gpio_set_dir_out_masked(15615);
  delay(2000);  // wait for modem.
  Serial.println();
  Serial.println("ready");
  print_pins();
  prompt();
}

void loop() {
  int pin = read_serial();
  if (pin >= 0) {
    flip_pin(pin);
  }
  delay(10);
}