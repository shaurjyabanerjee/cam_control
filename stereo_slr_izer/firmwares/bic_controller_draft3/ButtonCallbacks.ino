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

}

void buttonTest_releasedDurationCallback(uint8_t pinIn, unsigned long duration)
{
  // handle released state

}
