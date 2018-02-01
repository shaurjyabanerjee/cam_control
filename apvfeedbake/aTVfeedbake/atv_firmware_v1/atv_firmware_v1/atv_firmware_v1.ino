#include <TVout.h>
#include <TVoutfonts/fontALL.h>
#include "schematic.h"
#include "TVOlogo.h"

TVout TV;

//--------------------------------------------------------------------------

int enter_state = 0;
int menu_state = 1;

const byte button1_pin = 13;
const byte button2_pin = 12;
const byte button3_pin = 11;
const byte button4_pin = 10;

const byte pot1_pin = A0;
const byte pot2_pin = A1;
const byte pot3_pin = A2;
const byte pot4_pin = A3;

byte button1_state = 0;
byte button2_state = 0;
byte button3_state = 0;
byte button4_state = 0;

int pot1_val = 0;
int pot2_val = 0;
int pot3_val = 0;
int pot4_val = 0;

int t = 69;

byte print_state = 0;

const byte button_delay = 300;

void setup() 
{
  
  //Set pinmode for digital pins
  pinMode(button1_pin, INPUT);
  pinMode(button2_pin, INPUT);
  pinMode(button3_pin, INPUT);
  pinMode(button4_pin, INPUT);

  for (int i = 0; i < 4; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }
  
  TV.begin(NTSC,120,96);
  splashscreen();
}

void loop() 
{
  control_poller();
  control_interpreter();
  gfx_handler();
}

//---------------------------------------------------------------------------------

void fast_bytebeat()
{
  while(button1_state != HIGH)
  {
    pot1_val = analogRead(pot1_pin);
    pot2_val = analogRead(pot2_pin);
    pot3_val = analogRead(pot3_pin);
    
    button1_state = digitalRead(button1_pin);

    byte shift_amt_0 = pot2_val >> 8;
    byte shift_amt_1 = pot2_val >> 6;
    byte shift_amt_2 = pot2_val >> 4;

    byte post_bbcalc_shift = pot3_val >> 7;
    
    t++;
  
    int bbval = ((t << shift_amt_0) ^ ((t << shift_amt_0) + 
    (t >> shift_amt_1) & t >> shift_amt_2)) | 
    t >> (4 - (1 ^ shift_amt_1 & (t >> shift_amt_2))) | 
    t >> shift_amt_1;
    
    bbval = bbval >> post_bbcalc_shift;
    
    bbval = bbval & 0b1111;
    
    PORTD = bbval;

    
    
    //pot1_val*= 1.0;
    delayMicroseconds(5 + pot1_val);
  }
  
}

void slow_bytebeat()
{
  
}

void gfx_handler()
{
  //MENU1 - FAST BYTEBEAT
  if (enter_state != 0 && menu_state == 1)
  {
    TV.clear_screen();
    fast_bytebeat_gfx();
  }

  //MENU2 - SLOW BYTEBEAT
  if (enter_state != 0 && menu_state == 2)
  {
    TV.clear_screen();
    slow_bytebeat_gfx();
  }

  //If we are not currently in a mode, 
  //Go back to main mode choosing menu
  if (enter_state == 0 && (button1_state == HIGH || button2_state == HIGH || button4_state == HIGH))
  {
    TV.clear_screen();
    display_menu();
    print_state = 0;
  }
}

void slow_bytebeat_gfx()
{
  TV.select_font(font8x8);
  TV.println("SLOW BYTEBEAT");

  if (button3_state == HIGH)
  {
    slow_bytebeat();
  }
}

void fast_bytebeat_gfx()
{
  TV.select_font(font8x8);
  TV.println("FAST BYTEBEAT");
  TV.println("");

  TV.select_font(font6x8);
  TV.println("((t << 1) ^ ((t << 1) + (t >> 7) & t >> 12)) | t >> (4 - (1 ^ 7 & (t >> 15))) | t >> 7");

  

//  if (button3_state == HIGH)
//  {
    fast_bytebeat();
//  }
}

void display_menu()
{
  TV.select_font(font8x8);
  TV.println("SELECT A MODE");
  
  TV.select_font(font6x8);
  TV.println("");

  //MENU ITEM 1 -
  if (menu_state == 1) {TV.println("FAST BYTEBEAT <----");}
  else if (menu_state != 1) {TV.println("FAST BYTEBEAT");}  

  //MENU ITEM 2 -
   if (menu_state == 2) {TV.println("SLOW BYTEBEAT <----");}
  else if (menu_state != 2) {TV.println("SLOW BYTEBEAT");}

  //MENU ITEM 3 -
  if (menu_state == 3) {TV.println("CV TV <-----------");}
  else if (menu_state != 3) {TV.println("CV TV");}

  //MENU ITEM 4 -
  if (menu_state == 4) {TV.println("MAGNETS <----------");}
  else if (menu_state != 4) {TV.println("MAGNETS");}

  //MENU ITEM 5 - 
  if (menu_state == 5) {TV.println("OSCILLOGRAFIX <----");}
  else if (menu_state != 5) {TV.println("OSCILLOGRAFIX");}

  delay(button_delay);
}

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

void control_interpreter()
{
  //BUTTON 4 - INCREMENTS MENU STATE
  if (button4_state == HIGH && enter_state == LOW)
  {
    menu_state ++;
    menu_state = menu_state%7;
    delay(button_delay);
  }
  //BUTTON 2 - DECREMENTS MENU STATE
  if (button2_state == HIGH && enter_state == LOW)
  {
    menu_state --;
    if (menu_state == 0) {menu_state = 7;}
    delay(button_delay);
  }
  //BUTTON 1 - ENTER
  if (button1_state == HIGH && enter_state == LOW)
  {
    enter_state = HIGH;
    delay(button_delay);
  }
  //BUTTON 3 - BACK
  else if (button3_state == HIGH && enter_state == HIGH)
  {
    enter_state = LOW;
    delay(button_delay);
  }
}

void splashscreen()
{
  TV.select_font(font8x8);
  TV.println("");
  TV.println("");
  TV.println("");
  TV.println("");
  TV.println(" APVFeedBakE");
  TV.delay(2000);
  TV.println("A FUxxeD UP TV");
  TV.println("");
  TV.delay(3000);

  TV.select_font(font6x8);
  TV.println("        2k18");
  delay(2000);
 
  TV.clear_screen();
}


