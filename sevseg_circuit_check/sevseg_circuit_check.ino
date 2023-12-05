#define MS_PER_DIGIT_A 3
#define MS_PER_DIGIT_B 3

uint32_t active_mask = 0;

void setup() {
  Serial.begin(115200);
  Serial.println("start");
  gpio_init_mask(15615);
  gpio_set_dir_out_masked(15615);
  for (int pin = 10; pin <= 13; pin++) {
    gpio_set_outover(pin, GPIO_OVERRIDE_INVERT);
  }
}

void update_mask(uint32_t mask) {
  if (mask != active_mask) {
    gpio_clr_mask(active_mask);
    active_mask = mask;
    gpio_set_mask(active_mask);
  }
}

void loop() {
  switch ((millis() % 2000) / 500) {
    case 0:
      switch ((millis() / MS_PER_DIGIT_A) % 4) {
        case 0:
          update_mask(95 + (1 << 13));
          break;
        case 1:
          update_mask(216 + (1 << 12));
          break;
        case 2:
          update_mask(216 + (1 << 11));
          break;
        case 3:
          update_mask(217 + (1 << 10));
      }
      break;
    case 2:
          switch ((millis() / MS_PER_DIGIT_B) % 4) {
        case 0:
          update_mask(209 + (1 << 13));
          break;
        case 1:
          update_mask(216 + (1 << 12));
          break;
        case 2:
          update_mask(218 + (1 << 11));
          break;
        case 3:
          update_mask(33 + (1 << 10));
      }
      break;
    default:
      update_mask(0);
  }
  delayMicroseconds(11);  //  arbitrary (not round) delay
}