#include "buttons.h"

// KEYFRAMER FUNCTIONS

//const int max_keyframe_amt = 20;
#define MAX_KEYFRAME_COUNT  20

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
    Serial.print(F("setting keyframe "));
    Serial.print(n_keyframes);
    Serial.print(F(": "));
    Serial.print(x);
    Serial.print(", ");
    Serial.print(p);
    Serial.print('\n');

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
    Serial.print(F("backwards "));
    Serial.print(diff);
    move_steps(diff, 0, 350, X_AXIS);
    Serial.print(F(" - done."));
    Serial.print('\n');
  } else if (dest->x >= x_position)  {
    Serial.print(F("forwards "));
    Serial.print(diff);
    move_steps(diff, 1, 350, X_AXIS);
    Serial.print(F(" - done."));
    Serial.print('\n');
  }
}

void keyframe_loop()
{
  switch (get_button_press(BUTTON_THREE)) {
    case BUTTON_PRESS_SHORT: {
      visit_keyframe_blocking(0);
      break;
    }
    case BUTTON_PRESS_LONG: {
      set_keyframe(0, x_position, p_position);
      break;
    }
    default: {
      break;
    }
  }

  // when one of the jog buttons is released, print the current positions
  if ((get_button_press(BUTTON_TWO) != BUTTON_PRESS_NONE) || (get_button_press(BUTTON_FOUR) != BUTTON_PRESS_NONE)) {

    Serial.print(x_position);
    Serial.print(' ');
    Serial.print(p_position);
    Serial.print('\n');
  }
}

void print_keyframes()
{
  Serial.print(n_keyframes);
  Serial.print(F(" keyframes:\n"));
  for (int i = 0; i < n_keyframes; i++) {
    Serial.print('\t');
    Serial.print(i);
    Serial.print(F(": "));
    Serial.print(keyframes[i].x);
    Serial.print(' ');
    Serial.print(keyframes[i].y);
    Serial.print('\n');
  }
}
