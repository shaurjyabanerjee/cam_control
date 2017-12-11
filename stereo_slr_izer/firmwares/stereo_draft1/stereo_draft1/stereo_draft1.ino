//Stereo DSL-izer Firmware V0.0
//Shaurjya Banerjee - 2017
//Untested!!

//LIBRARIES --------------------------------------------------------------



//PINS -------------------------------------------------------------------

int shutter_pin = 13;
int focus_pin   = 12;
int en_pin      = 11;
int dir_pin     = 10;
int pulse_pin   = 9;
int screen_rst  = 8;
int limit_pin   = 7;

//VARIABLES --------------------------------------------------------------

int limit_status  = 0;
int focus_delay   = 200;
int shutter_delay = 200;

//MAIN SKETCH -----------------------------------------------------------

void setup()
{
  //OUTPUT PINS
  pinMode(shutter_pin, OUTPUT);
  pinMode(focus_pin,   OUTPUT);
  pinMode(en_pin,      OUTPUT);
  pinMode(dir_pin,     OUTPUT);
  pinMode(pulse_pin,   OUTPUT);
  
  //INPUT PINS
  pinMode(limit_pin, INPUT);
  
}

void loop()
{
  
}

//FUNCTIONS -------------------------------------------------------------

//Function to run homing routine
void homing_routine()
{
  limit_status = digitalRead(limit_pin);
  
  
}

//Fucntion to take a stereo picture
void take_stereo_photo(int separation)
{
  
}

//Function to trigger dSLR focus
void trigger_focus()
{
  digitalWrite(focus_pin, HIGH);
  delay(focus_delay);
  digitalWrite(focus_pin, LOW);
}

//Function to trigger dSLR shutter
void trigger_shutter()
{
  digitalWrite(shutter_pin, HIGH);
  delay(shutter_delay);
  digitalWrite(shutter_pin, LOW);
}

//Function to drive stepper motor a single step
void drive_motor(int dir)
{
  //To drive motor forward
  if (dir == 0)
  {
    digitalWrite(dir_pin, LOW);
    digitalWrite(en_pin, HIGH);
    digitalWrite(pulse_pin, HIGH);
    delayMicroseconds(50);
    digitalWrite(pulse_pin, LOW);
    delayMicroseconds(50);
  }
  //To drive motor backwards
  if (dir == 1)
  {
    digitalWrite(dir_pin, HIGH);
    digitalWrite(en_pin, HIGH);
    digitalWrite(pulse_pin, HIGH);
    delayMicroseconds(50);
    digitalWrite(pulse_pin, LOW);
    delayMicroseconds(50);
  }
}

//Function to stop stepper motor but maintain torque
void stop_motor()
{
  
}
