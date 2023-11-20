#pragma once

// Tracks dial movement and push-button state in a rotary encoder.
class RotaryEncoder {
public:
  // pinA, pinB: dial rotation; pinC: push-button
  RotaryEncoder(int pinA, int pinB, int pinC);

  // Track dial state; must be called by client in loop().
  void update();

  // Net count of detent movements left (-) or right (+) dial has moved
  // since encoder was zeroed.
  int movement() const {
    return movement_;
  }

  bool in_detent() const {
    return rotationState_ == NEUTRAL;
  }

  // Reset movement.
  void zero();

  // Report if the push-button is pressed.
  bool is_pressed() const;

  int rotation_state();

private:
  // RotationState tracks whether the dial is making a left or right movement
  // and where it is in that movement.
  enum RotationState {  // [(last RotationState, pin B, pin B)]
    NEUTRAL,            // 0: (*, 1, 1)
    LEFT_1,             // 1: ({NEUTRAL, LEFT_2}, 1, 0)
    LEFT_2,             // 2: ({LEFT_1, LEFT_3}, 0, 0)
    LEFT_3,             // 3: (LEFT_2, 0, 1)
    RIGHT_1,            // 4: ({NEUTRAL, RIGHT_2}, 0, 1)
    RIGHT_2,            // 5: ({RIGHT_1, RIGHT_3}, 0, 0)
    RIGHT_3,            // 6: (RIGHT_2, 1, 0)
    UNKNOWN,            // 7: any other condition
    
  };
  RotationState rotationState_;

  // Store movement count.
  long movement_;

  // Pins A & B encode rotation. Both are HIGH in detent.
  int pinA_, pinB_;

  // Pin C encodes if push-button is in detent (HIGH) or being pressed (LOW).
  int pinC_;
};
