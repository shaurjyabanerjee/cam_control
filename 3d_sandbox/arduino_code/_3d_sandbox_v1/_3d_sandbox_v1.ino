//3D SANDBOX FIRMWARE V1

//New Firmware for APVFeedBakE camera controller for 
//live moving camera anaglyph display.
//Mechanism homes to user defined limits, self regulates rotation.
//Sends serial to Max to describe rotation 'state'

//All stated features tested and working

//Shaurjya Banerjee - 2017

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

//APVFeedBake CNC Controller Pin Mapping 

const int tilt_left_pin  = 4;
const int tilt_right_pin = 5;
const int pan_left_pin   = 7;
const int pan_right_pin  = 6;
const int manual_pin     = 8;
const int auto_pin       = 9;
const int cycle_pin      = 10;
const int speed_pin      = A0;

//Control Variables

const int step_type = MICROSTEP;
const int cycle_step_type = MICROSTEP;
const int speed_val = 1;

int pan_left_state;
int pan_right_state;
int tilt_left_state;
int tilt_right_state;
int run_state;

int origin_state;
int origin_count = 0;
int origin_delay = 750;

int limit_counter;
int current_step;
int homed_state;

//0 = Going Left 
//1 = Going Right
int dir_state; 

int l_limit;
int r_limit;
int step_delay;

//Motor Setup
//Written for two motors to accomodate the stacked sheild arrangement
//used in APVFeedBakE

Adafruit_MotorShield AFMS1 = Adafruit_MotorShield(0x60);
Adafruit_MotorShield AFMS2 = Adafruit_MotorShield(0x61);
Adafruit_StepperMotor *tiltMotor = AFMS1.getStepper(200, 2);
Adafruit_StepperMotor *panMotor  = AFMS2.getStepper(200, 2);


void setup ()
{
  Serial.begin(9600);
  
  //Set input pinmode for control pins
  pinMode(tilt_left_pin , INPUT);
  pinMode(tilt_right_pin, INPUT);
  pinMode(pan_left_pin  , INPUT);
  pinMode(pan_right_pin , INPUT);
  pinMode(manual_pin    , INPUT);
  pinMode(auto_pin      , INPUT);
  pinMode(cycle_pin     , INPUT);
  
  //Set up motor libraries
  AFMS1.begin();
  AFMS2.begin();
  tiltMotor ->setSpeed(0);
  panMotor  ->setSpeed(0);
}

void loop ()
{
  read_inputs();
  handle_pan_buttons();
  handle_tilt_buttons();
  homing_routine();
  cycle_start(step_type);
  send_state(dir_state);
}

//Function to read all input pins from CNC controller 
void read_inputs()
{
  //Read in analog and digital inputs for control
  pan_left_state   = digitalRead(pan_left_pin);
  pan_right_state  = digitalRead(pan_right_pin);
  origin_state     = digitalRead(cycle_pin);
  tilt_left_state  = digitalRead(tilt_left_pin);
  tilt_right_state = digitalRead(tilt_right_pin);
  run_state        = digitalRead(manual_pin);
  step_delay       = analogRead(speed_pin);
}

//Function to call homing routine for user to input 'limits'
//User must first jog the device over to its left limit,
//Hold down the origin select button, while jogging the device 
//over to the right limit,then releasing the origin button 
//to set the right limit.
void homing_routine()
{
  //If the origin button is pressed
  if (origin_state == HIGH)
  {
    //Wait a little so that we dont re-trigger
    //delay(origin_delay);
    
    //Count steps while jogging
    if (pan_right_state == HIGH)
    {
      origin_count++; 
    }
    //Print counted steps for debugging
    //Serial.println(origin_count);
  }
  //If the limit has been set,make homed state high
  if (origin_count > 0)
  {
    homed_state = 1;
    limit_counter = origin_count;
  }
}

//Function to center the camera on user defined limit
void center_on_limit()
{
  
}

//Function to send rotation state to Max
void send_state(int direction_state)
{
  if (direction_state == 0)
  {
    Serial.println("Going Left!");
  }
  else if (direction_state == 1)
  {
    Serial.println("Going Right!");
  }
}

//Function to handle pan buttons for manual jog
void handle_pan_buttons()
{ 
  //If the left pan button is engaged
  if (pan_left_state == HIGH && pan_right_state == LOW)
  {
    panMotor->setSpeed(speed_val);
    panMotor->step(1, FORWARD, step_type);
    panMotor->setSpeed(0);
    dir_state = 0;
    
    // if the system has already been homed
    if (homed_state == HIGH)
    {
       //Decrement global step counter
       current_step--; 
    }   
  }
  //If the right pan button is engaged
  if (pan_left_state == LOW && pan_right_state == HIGH)
  {
    panMotor->setSpeed(speed_val);
    panMotor->step(1, BACKWARD, step_type);
    panMotor->setSpeed(0); 
    dir_state = 1;
    
    // if the system has already been homed
    if (homed_state == HIGH)
    {
       //Increment global step counter
       current_step++; 
    }
  }
  //Apply delay 
  delay(step_delay);
}

//Function to handle tilt buttons for manual jog
void handle_tilt_buttons()
{ 
  //If the left pan button is engaged
  if (tilt_left_state == HIGH && tilt_right_state == LOW)
  {
    tiltMotor->setSpeed(speed_val);
    tiltMotor->step(1, FORWARD, step_type);
    tiltMotor->setSpeed(0);
  }
  //If the right pan button is engaged
  if (tilt_left_state == LOW && tilt_right_state == HIGH)
  {
    tiltMotor->setSpeed(speed_val);
    tiltMotor->step(1, BACKWARD, step_type);
    tiltMotor->setSpeed(0); 
  }
  //Apply delay 
  delay(step_delay);
}

//Function to initialte cycling motion
void cycle_start(int cycle_step_type)
{
  if (run_state == HIGH && homed_state == HIGH && origin_state == LOW)
  {
    for (int i = current_step; i < limit_counter; i++)
    {
      panMotor->setSpeed(speed_val);
      panMotor->step(1, BACKWARD, cycle_step_type);
      panMotor->setSpeed(0);
      //Increment global step counter
      current_step++;
      dir_state = 1;
      send_state(dir_state);
    }
    for (int i = current_step; i > 0; i--)
    {
      panMotor->setSpeed(speed_val);
      panMotor->step(1, FORWARD, cycle_step_type);
      panMotor->setSpeed(0);
      //Decrement global step counter
      current_step--;
      dir_state = 0;
      send_state(dir_state);
    }
  }
}

//Function to print key limit values for debugging
void debug_printer()
{
  //Print for debugging
  Serial.print(current_step);
  Serial.print(",");
  Serial.print(limit_counter);
  Serial.print(",");
  Serial.println();
}

