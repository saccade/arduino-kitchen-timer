#include <map>

// Store pin values.
std::map<int, bool> pins;

// Bitmask (binary code) for updating pins.
int full_mask = 0;

// Encode pin state as a bitmask.
int get_mask(bool full_mask = false) {
  int mask = 0;
  for (auto& [pin, val] : pins) {
    mask += (val or full_mask) * round(pow(2, pin));
  }
  return mask;
}

// Refresh display.
void update_mask() {
  // These functions behave just like flipping the pins one at a time would,
  // but work many times faster.
  gpio_clr_mask(full_mask);
  gpio_set_mask(get_mask());
}

// Tell user how to proceed.
void prompt() {
  Serial.println("enter pincode (0-9 + AB; P to print; M for bitmask):");
}

// Read modem input; dispatch side effects; return GPIO pin to flip if any.
int read_serial() {
  if (not Serial.available()) return -1;  // nothing.

  String input = Serial.readString();
  if (not input.length()) return -1;  // empty.

  // An input.  Using just the first character is straightforward here.
  char pincode = input[0];
  // Regularize case.
  if ('a' <= pincode and pincode <= 'z') {
    pincode = pincode + 'A' - 'a';
  }
  Serial.print("pincode: ");
  Serial.println(pincode);

  // pin values
  if ('0' <= pincode and pincode <= '9') {
    return pincode - '0' + 10;
  } else if (pincode == 'A' or pincode == 'B') {
    return pincode - 'A' + 20;

  // messages
  } else if (pincode == 'P') {
    print_pins();
  } else if (pincode == 'M') {
    print_mask();
  } else {
    Serial.print("couldn't read input: " + input);
    prompt();
  }

  // In C, false is a valid pin number, and there is no None.
  return -1;
}

// Time to make the switch!
void flip_pin(int pin) {
  pins[pin] = not pins[pin];
  Serial.println("pin " + String(pin) + ": " + String(pins[pin] ? "HIGH" : "LOW"));
}

// Human-readable equivalent of mask.
void print_pins() {

  // low pins 
  Serial.print("LOW: ");
  for (auto& [pin, val] : pins) {
    if (not val) {
      Serial.print(pin);
      Serial.print(" ");
    }
  }
  Serial.println();

  // high pins
  Serial.print("HIGH: ");
  for (auto& [pin, val] : pins) {
    if (val) {
      Serial.print(pin);
      Serial.print(" ");
    }
  }
  Serial.println();
}

// I used this to write the circuit check program.
void print_mask() {
  Serial.print("mask: ");
  Serial.println(get_mask());
}

void setup() {
  Serial.begin(115200);
  delay(1000);  // wait for the modem.
  Serial.println("start");

  // Set up Pico pins 10-21 (the lowest 12 non-ground pins on the board.)
  for (int i = 10; i <= 21; i++) {
    pins[i] = false;
  }

  full_mask = get_mask(true);

  // Set all pins to be addressed by bit mask (binary code).
  gpio_init_mask(full_mask);
  gpio_set_dir_out_masked(full_mask);
  
  // Ready to go.
  update_mask();
  print_pins();
  prompt();
}

void loop() {
  // Check for input.
  int pin = read_serial();

  // Non-negative means a valid pin code.
  if (pin >= 0) {
    // Change one pin.
    flip_pin(pin);
  }

  // Update the display every time.
  update_mask();

  // Wait just a moment. (It won't be long.)
  delay(11);
}