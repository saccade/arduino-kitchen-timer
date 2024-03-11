#define MS_PER_DIGIT_A 3
#define MS_PER_DIGIT_B 100

#define FULL_CYCLE_MS 2000  // repeat every 2 seconds

uint32_t active_mask = 4193280;

void setup() {
  Serial.begin(115200);
  Serial.println("start");
  gpio_init_mask(active_mask);
  gpio_set_dir_out_masked(active_mask);
}

void update_mask(uint32_t mask) {
  if (mask != active_mask) {
    gpio_clr_mask(active_mask);
    active_mask = mask;
    gpio_set_mask(active_mask);
  }
}

void loop() {
  switch ((millis() % FULL_CYCLE_MS) / (FULL_CYCLE_MS / 4)) {
    case 0:
      switch ((millis() / MS_PER_DIGIT_A) % 4) {
        case 0:
          update_mask(2010112);
          break;
        case 1:
          update_mask(2288640);
          break;
        case 2:
          update_mask(2419712);
          break;
        case 3:
          update_mask(2583552);
      }
      break;
    case 2:
          switch ((millis() / MS_PER_DIGIT_B) % 4) {
        case 0:
          update_mask(502784);
          break;
        case 1:
          update_mask(2288640);
          break;
        case 2:
          update_mask(2944000);
          break;
        case 3:
          update_mask(2560000);
      }
      break;
    default:
      update_mask(0);
  }
  delayMicroseconds(11);  //  arbitrary (not round) delay
}