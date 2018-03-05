//APVFeedBakE CRT Firmware V0.0.0
//James Anderson and Shaurjya Banerjee 2018

#include <TVout.h>
#include <TVoutfonts/fontALL.h>
#include "schematic.h"
#include "TVOlogo.h"

TVout TV;

//VARIABLES-------------------------------------------------------------------------

byte enter_state = 0;
byte menu_state = 1;
byte print_state = 0;

byte osc_state = 0;

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
byte trig1_state = 0;
byte trig2_state = 0;

int pot1_val = 0;
int pot2_val = 0;
int pot3_val = 0;
int pot4_val = 0;

const byte relayh  = 0;
const byte relayv  = 1;
const byte relaym1 = 2;
const byte relaym2 = 3;

const byte trig1_pin = 5;
const byte trig2_pin = 6;

int t = 69;

const byte button_delay = 300;

void setup() 
{ 
  pinMode(button1_pin, INPUT);
  pinMode(button2_pin, INPUT);
  pinMode(button3_pin, INPUT);
  pinMode(button4_pin, INPUT);
  pinMode(trig1_pin, INPUT);
  pinMode(trig2_pin, INPUT);

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

void bytebeat_le()
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
    
    delayMicroseconds(5 + pot1_val);
  } 
}

void bytebeat_editor()
{
  
}

void oscillopgraphics(byte state)
{
  if (state == HIGH)
  {
    digitalWrite(relayh, LOW);
    digitalWrite(relayv, LOW);
  }
  else if (state == LOW)
  {
    digitalWrite(relayh, HIGH);
    digitalWrite(relayv, HIGH);
  } 
}

void cv_tv()
{
  if (trig1_state == LOW)
  {
    digitalWrite(relayh, LOW);
    digitalWrite(relayv, LOW);
  }

  if (trig1_state == HIGH)
  {
    digitalWrite(relayh, HIGH);
    digitalWrite(relayv, HIGH);
  }

  if (trig2_state == LOW)
  {
    digitalWrite(relaym1, LOW);
    digitalWrite(relaym2, LOW);
  }

  if (trig2_state == HIGH)
  {
    digitalWrite(relaym1, HIGH);
    digitalWrite(relaym2, HIGH);
  }
}


//GFX------------------------------------------------------------------------------

void gfx_handler()
{
  //MENU1 - Bytebeat LE
  if (enter_state != 0 && menu_state == 1)
  {
    TV.clear_screen();
    bytebeat_le_gfx();
  }

  //MENU2 - BYTEBEAT EDITOR
  if (enter_state != 0 && menu_state == 2)
  {
    TV.clear_screen();
    bytebeat_editor_gfx();
  }

  //MENU3 - CV TV 
  if (enter_state != 0 && menu_state == 3)
  {
    TV.clear_screen();
    cv_tv_gfx();
  }

  //MENU4 - MAGNETS
  if (enter_state != 0 && menu_state == 4)
  {
    TV.clear_screen();
    
  }

  //MENU5 - OSCILLOGRAPHIX
  if (enter_state != 0 && menu_state == 5)
  {
    TV.clear_screen();
    oscillopgraphics(HIGH);
  }

  //MENU6 - DEBUG
  if (enter_state != 0 && menu_state == 6)
  {
    TV.clear_screen();
    oscillopgraphics(HIGH);
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

void bytebeat_editor_gfx()
{
  TV.select_font(font8x8);
  TV.println("BYTEBEAT EDITOR");

  if (button3_state == HIGH)
  {
    bytebeat_editor();
  }
}

void bytebeat_le_gfx()
{
  TV.select_font(font8x8);
  TV.println("BYTEBEAT LE");
  TV.println("");

  TV.select_font(font6x8);
  TV.println("((t << 1) ^ ((t << 1) + (t >> 7) & t >> 12)) | t >> (4 - (1 ^ 7 & (t >> 15))) | t >> 7");

  bytebeat_le();
}

void cv_tv_gfx()
{
  TV.select_font(font8x8);
  TV.println("");
  TV.println("");
  TV.println("");
  TV.println("CV TV");
  cv_tv();
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

  //MENU ITEM 6 - 
  if (menu_state == 6) {TV.println("DEBUG <------------");}
  else if (menu_state != 6) {TV.println("DEBUG");}

  delay(button_delay);
}

void control_poller()
{
  button1_state = digitalRead(button1_pin);
  button2_state = digitalRead(button2_pin);
  button3_state = digitalRead(button3_pin);
  button4_state = digitalRead(button4_pin);
  trig1_state = digitalRead(trig1_pin);
  trig2_state = digitalRead(trig2_pin);

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
  if (button3_state == HIGH && enter_state == LOW)
  {
    enter_state = HIGH;
    delay(button_delay);
  }
  //BUTTON 3 - BACK
  else if (button1_state == HIGH && enter_state == HIGH)
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
 
  TV.clear_screen();
}


