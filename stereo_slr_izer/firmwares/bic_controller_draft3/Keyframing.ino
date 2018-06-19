#include "buttons.h"

// KEYFRAMER FUNCTIONS

//const int max_keyframe_amt = 20;
#define MAX_KEYFRAME_COUNT  1

int n_keyframes = 0;

struct Keyframe {
  int x;
  int y;
};

Keyframe keyframes[MAX_KEYFRAME_COUNT];

void set_keyframe(int index, int x, int p)
{
  static Keyframe new_keyframe;
  if (n_keyframes < MAX_KEYFRAME_COUNT) {
    new_keyframe.x = x;
    new_keyframe.y = p;
    keyframes[n_keyframes] = new_keyframe;
    n_keyframes++;
  } else {
    Serial.println(F("tried to set keyframes when the array is full"));
  }
}

void visit_keyframe_blocking(int index)
{
  Keyframe *dest = &keyframes[index];

  Serial.print(F("going to keyframe at position: "));
  Serial.print(dest->x);
  Serial.print(' ');
  Serial.print(dest->y);
  Serial.print(' ');
  Serial.print('\n');

  int diff = dest->x - x_position;

  if (dest->x < x_position) {
    Serial.print(F("(backwards"));
    move_steps(diff, 0, 350, X_AXIS);
  } else if (dest->x >= x_position)  {
    Serial.print(F("(forwards)"));
    move_steps(diff, 1, 350, X_AXIS);
  }
}

void keyframe_loop()
{
  switch (get_button_press(BUTTON_THREE)) {
    case BUTTON_PRESS_SHORT: {
      Serial.println("go to (next) keyframe (?) NYI");
      break;
    }
    case BUTTON_PRESS_LONG: {
      Serial.println("add keyframe at current position (NYI)");
      break;
    }
    default: {
      break;
    }
  }
}
