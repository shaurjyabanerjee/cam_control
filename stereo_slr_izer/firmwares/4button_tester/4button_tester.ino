//CONTROLLER PINS -----------------------------------------------------------------
int button1_pin = 2;
int button2_pin = 3;
int button3_pin = 4;
int button4_pin = 5;

int pot1_pin = A0;
int pot2_pin = A1;
int pot3_pin = A2;
int pot4_pin = A3;

//VARIABLES -----------------------------------------------------------------------
int button1_state = 0;
int button2_state = 0;
int button3_state = 0;
int button4_state = 0;

int pot1_val = 0;
int pot2_val = 0;
int pot3_val = 0;
int pot4_val = 0;


void setup()
{
   Serial.begin(9600);
   //Set pinmode for controller pins
   pinMode(button1_pin, INPUT);
   pinMode(button2_pin, INPUT);
   pinMode(button3_pin, INPUT);
   pinMode(button4_pin, INPUT);
}

void loop()
{
  control_poller();
  Serial.print(pot1_val);
  Serial.print(pot2_val);
  Serial.print(pot3_val);
  Serial.println(pot4_val);
  delay(10);
}

//CONTROLLER FUNCTIONS
void control_poller()
{
  button1_state = digitalRead(button1_pin);
  button2_state = digitalRead(button2_pin);
  button3_state = digitalRead(button3_pin);
  button4_state = digitalRead(button4_pin);

  pot1_val = analogRead(pot1_pin);
  pot2_val = analogRead(pot2_pin);
  pot3_val = analogRead(pot3_pin);
  pot4_val = analogRead(pot4_pin);
}

