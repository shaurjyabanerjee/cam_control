//APVFeedBakE CRT Firmware V0.0.0
//James Anderson and Shaurjya Banerjee 2018

#include <TVout.h>
#include <TVoutfonts/fontALL.h>

TVout TV;

int n[9] = { 1, 1, 1, 1, 1, 1, 1, 1, 1, };

//VARIABLES-------------------------------------------------------------------------

byte enter_state = 1;
byte menu_state = 6;
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

const int RELAY_MASK = 0b10011111;

// demo modes-----------------------------------------------------------
int demo_timer = 0;
int demo_mode = 0;
int this_time = 0;
int last_time = 0;

const int DEMO_TIME = 10000; // ms
// ---------------------------------------------------------------------

void setup() 
{ 
  pinMode(button1_pin, INPUT);
  pinMode(button2_pin, INPUT);
  pinMode(button3_pin, INPUT);
  pinMode(button4_pin, INPUT);
  pinMode(trig1_pin, INPUT);
  pinMode(trig2_pin, INPUT);

  for (int i = 0; i < 6; i++) {
    pinMode(i, OUTPUT);
    digitalWrite(i, HIGH);
  }

  pinMode(8, OUTPUT);

  enable_menu();

  
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

    bytebeat_viz(bbval);
    
    bbval = bbval >> post_bbcalc_shift;
    bbval = bbval & RELAY_MASK;
    PORTD = bbval;
    PORTB = (bbval >> 2) & 1;
    
    delayMicroseconds(5 + pot1_val);
  } 
}

void bb_single()
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

void demo() {
    last_time = this_time;
    this_time = millis();

    demo_timer += (this_time - last_time);

    bool splash = true;

    switch (demo_mode) {
        case 0: {
              video1();
            break;
        }
        case 1: {
            video2();
            break;
        }
        case 2: {
            oscillopgraphics(HIGH);
            digitalWrite(2, HIGH);
            digitalWrite(3, HIGH);
            break;
        }
        case 3: {
            enable_menu();
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
            
                bytebeat_viz(bbval);
                
                bbval = bbval >> post_bbcalc_shift;
                bbval = bbval & RELAY_MASK;
                PORTD = bbval;
                PORTB = (bbval >> 2) & 1;
                
                delayMicroseconds(5 + pot1_val);
            break;
        }

        case 4: {
            enable_menu();  
            if (splash) {
                splashscreen();
                splash = false;
            }
            break;
        }
        case 5: {
              enable_menu();  
              splash = true; // on the next cycle show the spash screen once
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
              
              bytebeat_viz(bbval);
              
              delay(5 + pot1_val);
            break;
        }
        default: {
            break;
        }
    }

    if (demo_timer > DEMO_TIME) {
        demo_timer = 0;
        demo_mode++;

        if (demo_mode > 5) {
            demo_mode = 0;
        }
    }
}

void video1()
{
    digitalWrite(0, HIGH);
    digitalWrite(1, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(8, HIGH);
}

void video2()
{
    digitalWrite(0, HIGH);
    digitalWrite(1, HIGH);
    digitalWrite(2, HIGH);
    digitalWrite(3, HIGH);
    digitalWrite(4, HIGH);
    digitalWrite(8, LOW);
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
//    bytebeat_editor_gfx();
  }

  //MENU3 - CV TV 
  if (enter_state != 0 && menu_state == 3)
  {
    video1();
  }

  //MENU4 - MAGNETS
  if (enter_state != 0 && menu_state == 4)
  {
    video2();
    
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
    demo();
  }

  

  //If we are not currently in a mode, 
  //Go back to main mode choosing menu
  if (enter_state == 0 && (button1_state == HIGH || button2_state == HIGH || button4_state == HIGH))
  {
    enable_menu();
    TV.clear_screen();
    display_menu();
    print_state = 0;
  }
}

void bytebeat_editor_gfx()
{
  TV.select_font(font8x8);
//  TV.println("BYTEBEAT EDITOR");

   
    pot1_val = analogRead(pot1_pin);
    pot2_val = analogRead(pot2_pin);

    byte selectionIndex = map(pot2_val, 0, 1024, 0, 8);
    n[selectionIndex] = map(pot1_val, 0, 1024, 0, 15);

    t++;

    for (int i = 0; i <= 8; i++)
    {
        TV.print(n[i]); TV.print(" ");
    }

    int bbval = ((t << n[0]) ^ ((t << n[1]) + (t >> n[2]) & t >> n[3])) | t >> (n[4] - (n[5] ^ n[6] & (t >> n[7]))) | t >> n[8];

    bbval = bbval & RELAY_MASK;

    // in demo mode, this might fuck up the other one (bytebeat le) since they both call this function
    // for whatever reason, this doesnt really do that much cool stuff anyway, so it clears out the screen
    // which makes the bytebeat le one not do as much cool stuff (or any)

#if 0
    bytebeat_viz(bbval);
#endif

    PORTD = bbval;
    
    delayMicroseconds(5 + pot1_val);
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
#if 1
void bytebeat_viz(int16_t n)
{
    // int bbval = ((t << 1) ^ ((t << 1) + (t >> 7) & t >> 12)) | t >> (4 - (1 ^ 7 & (t >> 15))) | t >> 7;
    int bbval = n;//((t * (t >> 8 | t >> 9) & 46 & t >> 8)) ^ (t & t >> 13 | t >> 6);
    control_poller();

    int8_t shiftx = 1, shifty = 1;

    uint8_t *dst = display.screen;
    uint8_t *src = display.screen + display.hres;
    uint8_t *end = display.screen + display.vres*display.hres;
    uint8_t tmp = 0;
    // int mode = map(pot1_val, 1024, 0, 0, 7);
    int mode = (bbval >> 1) & 7;
    switch (mode) {
//        case 0: {
//            TV.screen[bbval & 0xff] ^= bbval >> 12;
//            break;
//        }
//        case 1: {
//            TV.screen[bbval >> 3] |= (bbval & 0xff);
//            break;
//        }
//        case 2: {
//            TV.screen[bbval >> 3] ^= (bbval & 0xff);
//            break;
//        }
        case 3: {
            while (src <= end) {
                tmp = *dst;
                *dst = *src;
                *src = tmp;
                dst++;
                src++;
            }
            break;
        }
//        case 4: {
//            TV.screen[bbval >> 3] = ~(bbval & 0xff);
//            break;
//        }
        case 5: {
            TV.select_font((const unsigned char*) 0x100);
            TV.write((char*)bbval, (bbval >> 3) & 7);
            break;
        }
        case 6: {
            TV.select_font(font6x8);
            TV.write((char*)bbval, bbval & 0xc);
            break;
        }
        case 7: {
            while (src <= end) {
                tmp = *dst;
                *dst = ~(*src);
                *src ^= tmp;
                dst++;
                src++;
            }
            break;
        }
        default: {
            break;
        }
    }
}
#endif

void cv_tv_gfx()
{
//  TV.select_font(font8x8);
//  TV.println("");
//  TV.println("");
//  TV.println("");
//  TV.println("CV TV");
//  cv_tv();
}

void enable_menu()
{
    digitalWrite(8, HIGH);
    digitalWrite(4, LOW);

    digitalWrite(relayh, HIGH);
    digitalWrite(relayv, HIGH);
}

void display_menu()
{
  TV.select_font(font8x8);
  TV.println("SELECT A MODE");
  
  TV.select_font(font6x8);
  TV.println("");

  //MENU ITEM 1 -
  if (menu_state == 1) {TV.println("BYTEBEAT <----");}
  else if (menu_state != 1) {TV.println("BYTEBEAT");}  

  //MENU ITEM 2 -
#if 0
   if (menu_state == 2) {TV.println("SLOW BYTEBEAT <----");}
  else if (menu_state != 2) {TV.println("SLOW BYTEBEAT");}
#endif

  //MENU ITEM 3 -
  if (menu_state == 3) {TV.println("VID 1 <-----------");}
  else if (menu_state != 3) {TV.println("VID 1");}

  //MENU ITEM 4 -
  if (menu_state == 4) {TV.println("VID 2 <----------");}
  else if (menu_state != 4) {TV.println("VID 2");}

  //MENU ITEM 5 - 
  if (menu_state == 5) {TV.println("OSCILLOGRAFIX <----");}
  else if (menu_state != 5) {TV.println("OSCILLOGRAFIX");}

  //MENU ITEM 6 - 
  if (menu_state == 6) {TV.println("DEMO <-----------");}
  else if (menu_state != 6) {TV.println("DEMO");}

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
  TV.clear_screen();
  TV.select_font(font8x8);
  TV.println(" ");  TV.println(" ");
  TV.println("  APVFeedBakE");
  TV.delay(3000);  
  for( int i = 0; i < 10; i++ ) {
    TV.fill(2);
    TV.delay(50);
  }
    TV.println("  2018");
  TV.delay(3000); 
}


