#ifndef BUTTONS
#define BUTTONS

/*
 * types of button presses and button names
 * 
 */

typedef enum {
  BUTTON_PRESS_NONE,
  BUTTON_PRESS_SHORT,
  BUTTON_PRESS_LONG,
  BUTTON_PRESS_IGNORE,
  NUM_EVENTS
} ButtonPress;

typedef enum {
  BUTTON_ONE    = 0,
  BUTTON_TWO    = 1,
  BUTTON_THREE  = 2,
  BUTTON_FOUR   = 3,

  BUTTON_COUNT
} Buttons;

#endif // BUTTONS