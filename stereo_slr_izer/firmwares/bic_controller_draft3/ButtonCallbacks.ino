#include "buttons.h"

const int long_press_time = 1000; // ms

const unsigned char button_pins[] = {
  2,
  3,
  5,
  6
};

// state slot for any digital pin, not just the button pins defined above.
ButtonPress states[0xf] = { BUTTON_PRESS_NONE };


ButtonPress get_button_press(int button)
{
  ButtonPress popped_state = states[ button_pins[button] ];
  states[ button_pins[button] ] = BUTTON_PRESS_NONE;
  return popped_state;
}


void buttonTest_pressedCallback(uint8_t pinIn)
{
  // handle pressed state
  if (pinIn == button1_pin) {
    button1_state = HIGH;
  } else if (pinIn == button2_pin) {
    button2_state = HIGH;
  } else if (pinIn == button3_pin) {
    button3_state = HIGH;
  } else if (pinIn == button4_pin) {
    button4_state = HIGH;
  }
}

void buttonTest_releasedCallback(uint8_t pinIn)
{
  // handle released state
  if (pinIn == button1_pin) {
    button1_state = LOW;
  } else if (pinIn == button2_pin) {
    button2_state = LOW;
  } else if (pinIn == button3_pin) {
    button3_state = LOW;
  } else if (pinIn == button4_pin) {
    button4_state = LOW;
  }
}

void buttonTest_pressedDurationCallback(uint8_t pinIn, unsigned long duration)
{
  // handle still pressed state

  // TODO: instigate long press state here, when that duration is reached,
  // whether the button was released or not
}

void buttonTest_releasedDurationCallback(uint8_t pinIn, unsigned long duration)
{
  // handle released state

  if (duration > long_press_time) {
    states[pinIn] = BUTTON_PRESS_LONG;
  } else {
    states[pinIn] = BUTTON_PRESS_SHORT;
  }
}
