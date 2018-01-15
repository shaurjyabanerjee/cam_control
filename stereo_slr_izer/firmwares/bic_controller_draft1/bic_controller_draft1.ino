//Beaver Imaging Co. Camera Controller Firmware
//V1.0.0
//Shaurjya Banerjee - 2017

//Written to use SSD1306 128x64 monochrome OLED display from Adafruit with I2C
//Motion control calibrated to use TB6600 Microstep Driver with a 1.8˚/step stepper
//TB6600 set to 2A Microstepping (400 steps/revolution)

//LIBRARIES -----------------------------------------------------------------------
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//SCREEN STUFF --------------------------------------------------------------------

#define OLED_RESET 4
Adafruit_SSD1306 display(OLED_RESET);

#define NUMFLAKES 1
#define XPOS 0
#define YPOS 0
#define DELTAY 0

//MOTOR CONTROL PINS --------------------------------------------------------------
const byte pul_pin     = 9;
const byte dir_pin     = 10;

//MOTOR CONTROL VARIABLES ---------------------------------------------------------
float test_delay = 0.4;
long int test_steps = 24000;
const int pulse_delay = 50; //In microseconds

//LINEAR DISTANCE VARIABLES -------------------------------------------------------
const int steps_per_inch = 1266;  //Number of steps to move one inch linearly
const float inter_focals [] =     //Selection of inter focal distances in inches
{0.5, 1, 2, 2.5, 2.8, 3, 4, 6, 8, 10, 12, 15};
const int max_steps = 18990;      //Maximum travel for 500mm  OpenBuilds C-Beam

//CAMERA CONTROL PINS -------------------------------------------------------------
const byte focus_pin   = 8;
const byte shutter_pin = 7;

//CONTROLLER PINS -----------------------------------------------------------------
const byte button1_pin = 2;
const byte button2_pin = 3;
const byte button3_pin = 5;
const byte button4_pin = 6;

const byte pot1_pin = A0;
const byte pot2_pin = A1;
const byte pot3_pin = A2;
const byte pot4_pin = A3;

//STATE VARIABLES -----------------------------------------------------------------
byte button1_state = 0;
byte button2_state = 0;
byte button3_state = 0;
byte button4_state = 0;

int pot1_val = 0;
int pot2_val = 0;
int pot3_val = 0;
int pot4_val = 0;

byte menu_state           = 0;
byte enter_state          = 0;
byte is_busy              = 0;
byte print_state          = 0;
byte shutter_speed_state  = 0;
byte inter_focal_state    = 0;
byte kill_timelapse_state = 0;
int temp_step_counter     = 0;
int interfocal_steps      = 0;
int cooldown_state        = 0;
int step_delay_state      = 0;
int numb_shots            = 0;
int interval_state        = 0;

const int intervals[] =
{3, 5, 7, 10, 12, 15, 20, 25, 30, 35, 40, 45, 50, 55, 60, 90, 120, 150, 180, 240};

const int shutter_speeds[] = 
{125, 250, 500, 750, 1000, 2000, 4000, 6000, 8000, 10000, 15000, 30000};

const int cooldowns[] = {0, 1000, 2000, 4000};

//MISC VARIABLES ------------------------------------------------------------------
const byte button_delay = 60;
const int  global_shutter_delay = 1000;
  
#if (SSD1306_LCDHEIGHT != 64)
#error(F("Height incorrect, please fix Adafruit_SSD1306.h!"));
#endif

//BITMAPS -------------------------------------------------------------------------

//Beaver Imaging Co. Logo Image 1
#define SBVR_HEIGHT 128
#define SBVR_GLCD_WIDTH  64 
static const unsigned char PROGMEM sbvr [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xfc, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x7f, 0xfe, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x1f, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x0f, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x07, 0x80, 0x00, 0x00, 
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xfc, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x01, 0xff, 0xff, 0x80, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x7f, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3f, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//Beaver Imaging Co. Logo Image 2
#define BBVR_HEIGHT 128
#define BBVR_GLCD_WIDTH  64
static const unsigned char PROGMEM bbvr [] = {
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xf0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0x8f, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfc, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x3f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x01, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xfb, 0xff, 0xfc, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x03, 0xf0, 0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xf0, 0x7f, 0xfe, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x3f, 0xff, 0xc0, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xc0, 0x0f, 0xff, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x80, 0x03, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x03, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0xff, 0x80, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x3f, 0x80, 0x00, 0x00, 
0x00, 0x0f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xf8, 0x00, 0x1f, 0x80, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x0f, 0x80, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x07, 0x80, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xff, 0xfe, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xff, 0xff, 0xe0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x1f, 0xff, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x0f, 0xff, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x07, 0xff, 0xff, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x03, 0xff, 0xf8, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0xff, 0xc0, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x1c, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

//SETUP & LOOP --------------------------------------------------------------------

void setup()
{
   //Set pinmode for digital pins
   pinMode(button1_pin, INPUT);
   pinMode(button2_pin, INPUT);
   pinMode(button3_pin, INPUT);
   pinMode(button4_pin, INPUT);
   pinMode(focus_pin,   OUTPUT);
   pinMode(shutter_pin, OUTPUT);
   pinMode(pul_pin, OUTPUT);
   pinMode(dir_pin, OUTPUT);

   //Set default states of digital outs
   digitalWrite(focus_pin, HIGH);
   digitalWrite(shutter_pin, HIGH);
   
   //Initialization Routine
   Serial.begin(9600);
   display.begin(SSD1306_SWITCHCAPVCC, 0x3D);  // initialize with the I2C addr 0x3D (for the 128x64)

   display.clearDisplay();
   delay(10);

   beaver_flashscreen(5, 100);
   beaver_splashscreen();
   //print_busy();
}

void loop()
{
  control_poller();
  control_interpreter();
  gfx_handler();
}

//CAMERA CONTROL FUNCTIONS --------------------------------------------------------

//Function to run simple intervalometer routine
void intervalometer(int num_shots, int interval, int shutter_delay)
{ 
  for (int i = 0; i < num_shots; i++)
  {
    //Take a shot
    take_shot(shutter_delay);

    //Wait for desired interval
    for (int j = 0; j < (interval-1); j++)
    {
      //Check for exit pin
      button4_state = digitalRead(button4_pin);
      if (button4_state == HIGH) {break;}
      
      //Print active timelapse state 
      print_active_timelapse_state(num_shots, (i+1), (interval-(j+1)) );
      delay(1000);
    }
    //Check for exit pin
    if (button4_state == HIGH) {break;}
  }
}

//Function to run panning intervalometer routine
void panning_intervalometer(int num_shots, int interval, int dist,  int sh_delay)
{
  int dist_per_step = dist/num_shots;

  for (int i = 0; i < num_shots; i++)
  {
    //Take a shot
    take_shot(sh_delay); 

    //Move required distance
    move_steps(dist_per_step, 1, 300);

    //Wait for the desired interval
    for (int j = 0; j < (interval-1); j++)
    {
      //Check for exit pin
      button4_state = digitalRead(button4_pin);
      if (button4_state == HIGH) {break;}
      
      print_active_timelapse_state(num_shots, (i+1), (interval-(j+1)) );
      delay(1000);
    }
    //Check for exit pin
    if (button4_state == HIGH) {break;}
  }
  //Reset to original position
  move_steps(dist, 0, 300);
}
  

//Function to run stereo photography routine
void take_stereo_photo(int dist, int sh_delay, int cooldown, int spd)
{
  is_busy = HIGH;
  print_busy();
  delay(500);

  //Take first shot
  take_shot(global_shutter_delay);
  delay(sh_delay);
  //Move required distance
  move_steps(dist, 1, spd);
  //Allow time to settle
  delay(cooldown);
  //Take second shot
  take_shot(global_shutter_delay);
  delay(sh_delay);
  //Return to origin
  move_steps(dist, 0, spd);

  is_busy = LOW;
  display.clearDisplay(); 
}

void video_slider()
{
  
}


//CONTROLLER FUNCTIONS ------------------------------------------------------------
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
  //BUTTON 1 - INCREMENTS MENU STATE
  if (button1_state == HIGH && enter_state == LOW)
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
  //BUTTON 3 - ENTER
  if (button3_state == HIGH)
  {
    enter_state = HIGH;
    delay(button_delay);
  }
  //BUTTON 4 - BACK
  if (button4_state == HIGH)
  {
    enter_state = LOW;
    delay(button_delay);
  }
}

//CUSTOM GFX FUNCTIONS ------------------------------------------------------------

//Main GFX and Main Menu Handler Routine
void gfx_handler()
{
  //MENU1 - STEREO PHOTOGRAPHY
  if (enter_state != 0 && menu_state == 1)
  {
    display.clearDisplay();
    stereo_photo_gfx();
  }

  //MENU2 - TIME LAPSE CONTROLLER
  if (enter_state != 0 && menu_state == 2)
  {
    display.clearDisplay();
    time_lapse_gfx();
  }

  //MENU3 - PANNING TIME LAPSE
  if (enter_state != 0 && menu_state == 3)
  {
    display.clearDisplay();
    panning_time_lapse_gfx();
    
  }

  //MENU4 - VIDEO SLIDER
  if (enter_state != 0 && menu_state == 4)
  {
    display.clearDisplay();
  }

  //MENU5 - BAR GRAPH
  if (enter_state != 0 && menu_state == 5)
  {
    display.clearDisplay();
    display_pot_values();
  }

  //MENU6 - MANUAL JOG
  if (enter_state != 0 && menu_state == 6)
  {
    if (print_state == 0)
    {
      display.clearDisplay();
      display.setCursor(0,0);
      display.setTextColor(WHITE);
      display.setTextSize(2);
      display.println(F("MANUAL JOG"));
      display.display();
      print_state = 1;
      delay(200);
    }
    manual_jog();
  }
  
  //If we are not currently in a mode, 
  //Go back to main mode choosing menu
  if (enter_state == 0)
  {
    display.clearDisplay();
    display_menu();
    print_state = 0;
  }
}

//Flashing Beaver Logo
void beaver_flashscreen(int num_flashes, int dly)
{
  for (int i = 0; i < num_flashes; i++)
  {
    display.drawBitmap(0, 0, sbvr, 128, 64, 1);
    display.display();
    delay(dly);
    display.clearDisplay();
    display.drawBitmap(0, 0, bbvr, 128, 64, 1);
    display.display();
    delay(dly);
    display.clearDisplay();
  }
}

//Stationary Beaver Logo
void beaver_logo()
{
  display.drawBitmap(0, 0, bbvr, 128, 64, 1);
  display.display();
  delay(1500);
  display.clearDisplay();
}

//Text Splashscreen for Beaver Imaging Co.
void beaver_splashscreen()
{
  //BEAVER
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println(F("BEAVER"));
  delay(50);
  display.display();
  //IMAGING
  display.println(F("IMAGING"));
  delay(50);
  display.display();
  //CO.
  display.println("CO.");
  delay(50);
  display.display();

  //Camera Controller Version
  display.setTextSize(1);
  display.setTextColor(WHITE);
  display.println(F("CAMERA CONTROLLER"));
  display.println(F("Version 1.0.0"));
  delay(750);
  display.display();
  delay(1850);
  display.clearDisplay();
}

//Function to print the "BUSY" screen
void print_busy()
{
  display.clearDisplay();
  //"Center" the cursor
  display.setCursor(31,22);
  display.setTextColor(WHITE);
  display.setTextSize(3);
  display.println(F("BUSY"));
  display.display();
}

//Function to handle displaying a progress bar for the time lapse controller 
void print_active_timelapse_state (int nshots, int current_shot, int time_to_shot)
{
  byte progress_bar = map(current_shot, 1, nshots, 1, 128);
  display.clearDisplay();
  
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println(F("TIME LAPSE"));
  display.println(F("  ACTIVE"));

  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.println();
  display.println();

  //Print number of shots elapsed
  display.print(F("SHOTS     "));
  display.print(current_shot);
  display.print(F("/"));
  display.println(nshots);

  //Print time left to next shot
  display.print(F("NEXT IN   "));
  display.print(time_to_shot);
  display.println(F("sec"));

  //Other graphical elements
  display.fillRect(0, 34, progress_bar, 9, WHITE);
  display.drawFastVLine(52, 47, 20, WHITE);
  display.display();
}

//Function to display main mode menu
void display_menu()
{
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println(F("MODE"));
  display.setTextSize(1);
  
  //MENU ITEM 1 - 
  display.setTextColor(WHITE);
  if (menu_state == 1) {display.setTextColor(BLACK, WHITE);}
  else if (menu_state != 1) {display.setTextColor(WHITE,BLACK);}
  display.println(F("STEREO PHOTOGRAPHY   "));

  //MENU ITEM 2 - 
  display.setTextColor(WHITE);
  if (menu_state == 2) {display.setTextColor(BLACK, WHITE);}
  else if (menu_state != 2) {display.setTextColor(WHITE,BLACK);}
  display.println(F("TIME LAPSE CONTROLLER"));

  //MENU ITEM 3 - 
  display.setTextColor(WHITE);
  if (menu_state == 3) {display.setTextColor(BLACK, WHITE);}
  else if (menu_state != 3) {display.setTextColor(WHITE,BLACK);}
  display.println(F("PANNING TIME LAPSE   "));

  //MENU ITEM 4 - 
  display.setTextColor(WHITE);
  if (menu_state == 4) {display.setTextColor(BLACK, WHITE);}
  else if (menu_state != 4) {display.setTextColor(WHITE,BLACK);}
  display.println(F("VIDEO SLIDER       "));

  //MENU ITEM 5 - 
  display.setTextColor(WHITE);
  if (menu_state == 5) {display.setTextColor(BLACK, WHITE);}
  else if (menu_state != 5) {display.setTextColor(WHITE,BLACK);}
  display.println(F("BAR GRAPH          "));

  //MENU ITEM 6 - 
  display.setTextColor(WHITE);
  if (menu_state == 6) {display.setTextColor(BLACK, WHITE);}
  else if (menu_state != 6) {display.setTextColor(WHITE,BLACK);}
  display.println(F("MANUAL JOG         "));
  
  display.display();
}

//Function to handle GFX for Stereo Photography Submenu
void stereo_photo_gfx()
{
  inter_focal_state   = map(pot1_val, 0, 900, 0, 10);
  shutter_speed_state = map(pot2_val, 0, 900, 0, 10);
  cooldown_state      = map(pot3_val, 0, 800, 0, 3);
  step_delay_state    = map(pot4_val, 0, 1024, 1100, 370);
  interfocal_steps    = inter_focals[inter_focal_state] * steps_per_inch;
  
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println(F("STEREO PIX"));

  //Write Submenu Items
  display.setTextSize(1);
  display.println(F(" "));
  visualize_interfocal_travel();
  display.println(F(" "));
  display.print(F("IF  DIST    "));
  inter_focal_state_handler();
  display.print(F("SHTR DLY    "));
  shutter_speed_state_handler();
  display.print(F("STL TIME    "));
  cooldown_state_handler();
  display.print(F("MO SPEED    "));
  step_delay_state_handler();
  display.drawFastVLine(60, 33, 40, WHITE);
  display.display();
  
  //Trigger stereo shot
  if (button1_state == HIGH)
  {
    take_stereo_photo(interfocal_steps, shutter_speeds[shutter_speed_state], 
    cooldowns[cooldown_state], step_delay_state);
  }
}

//Function to wrap the selection and display of shutter speeds
void shutter_speed_state_handler()
{
  if (shutter_speed_state == 0)       {display.println(F("1/8 sec"));}
  else if (shutter_speed_state == 1)  {display.println(F("1/4 sec"));}
  else if (shutter_speed_state == 2)  {display.println(F("1/2 sec"));}
  else if (shutter_speed_state == 3)  {display.println(F("3/4 sec"));}
  else if (shutter_speed_state == 4)  {display.println(F("1 sec"));}
  else if (shutter_speed_state == 5)  {display.println(F("2 sec"));}
  else if (shutter_speed_state == 6)  {display.println(F("4 sec"));}
  else if (shutter_speed_state == 7)  {display.println(F("6 sec"));}
  else if (shutter_speed_state == 8)  {display.println(F("8 sec"));}
  else if (shutter_speed_state == 9)  {display.println(F("10 sec"));}
  else if (shutter_speed_state == 10) {display.println(F("15 sec"));}
  else if (shutter_speed_state == 11) {display.println(F("30 sec"));}
}

//Fucntion to wrap the selection and display of interfocal distances
void inter_focal_state_handler()
{
  if (inter_focal_state == 0)       {display.println(F("0.5 inch"));}
  else if (inter_focal_state == 1)  {display.println(F("1 inch"));}
  else if (inter_focal_state == 2)  {display.println(F("2 inch"));}
  else if (inter_focal_state == 3)  {display.println(F("2.5 inch "));}
  else if (inter_focal_state == 4)  {display.println(F("2.8 inch"));}
  else if (inter_focal_state == 5)  {display.println(F("3 inch"));}
  else if (inter_focal_state == 6)  {display.println(F("4 inch"));}
  else if (inter_focal_state == 7)  {display.println(F("6 inch"));}
  else if (inter_focal_state == 8)  {display.println(F("8 inch"));}
  else if (inter_focal_state == 9)  {display.println(F("10 inch"));}
  else if (inter_focal_state == 10) {display.println(F("12 inch"));}
  else if (inter_focal_state == 11) {display.println(F("15 inch"));}
}

//Function to wrap the display and selection of cooldown times
void cooldown_state_handler()
{
  if (cooldown_state == 0)      {display.println(F("0 sec"));}
  else if (cooldown_state == 1) {display.println(F("1 sec"));}
  else if (cooldown_state == 2) {display.println(F("2 sec"));}
  else if (cooldown_state == 3) {display.println(F("4 sec"));}
}

//Function to wrap the selection and display of movement speeds
void step_delay_state_handler()
{
  if (step_delay_state >= 700) {display.println(F("SLOW"));}
  else if (step_delay_state < 700 && step_delay_state > 500) {display.println(F("MEDIUM"));}
  else if (step_delay_state <= 500) {display.println(F("FAST!"));}
}

//Fucntion to wrap the display of a bar graph to visualize InterFocal travel
void visualize_interfocal_travel()
{
  byte temp = map(interfocal_steps, 0, max_steps, 0, 126);
  display.fillRect(0, 19, temp, 8, WHITE);
}

//Fucntion to wrap the display of a bar graph to visualize panning travel
void visualize_panning_travel()
{
  byte temp = map(interfocal_steps, 0, max_steps, 0, 126);
  display.fillRect(0, 19, temp, 8, WHITE);
}

//Function to handle GFX for time lapse submenu
void time_lapse_gfx()
{
  numb_shots     = map(pot1_val, 0, 1010, 1, 80);
  interval_state = map(pot2_val, 0, 1000, 0, 19);
  numb_shots = numb_shots * 25; 
  
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println(F("TIME LAPSE"));

  //Write Submenu Items
  display.setTextSize(1);
  display.println();
  display.print(F("NUM SHOTS   "));
  display.print(numb_shots);
  display.println();
  display.println();
  display.print(F("INTERVAL    "));
  interval_state_handler();
  display.println();
  display.print(F("TOTAL TIME  "));
  total_time_handler();
  display.drawFastVLine(65, 23, 50, WHITE);
  display.display();

  if (button1_state == HIGH)
  {
    intervalometer(numb_shots, intervals[interval_state], global_shutter_delay);
  }
}

//Function to handle the GFX for the panning time lapse submenu
void panning_time_lapse_gfx()
{
  numb_shots          = map(pot1_val, 0, 1010, 1, 80);
  interval_state      = map(pot2_val, 0, 1000, 0, 19);
  inter_focal_state   = map(pot3_val, 0, 900, 0, 10);
  interfocal_steps    = inter_focals[inter_focal_state] * steps_per_inch;
  numb_shots = numb_shots * 25; 
  
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println(F("P.TIMELAPS"));

  //Write Submenu Items
  display.setTextSize(1);
  display.println();
  display.println();
  display.print(F("NUM SHOTS   "));
  display.print(numb_shots);
  display.println();
  display.print(F("INTERVAL    "));
  interval_state_handler();
  display.print(F("DISTANCE    "));
  inter_focal_state_handler();
  display.print(F("TOTAL TIME  "));
  total_time_handler();
  visualize_panning_travel();
  display.drawFastVLine(65, 30, 33, WHITE);
  
  display.display();
  
  if (button1_state == HIGH)
  {
    panning_intervalometer(numb_shots, intervals[interval_state], interfocal_steps, global_shutter_delay);
  }
}

//Function to wrap the selection and display of intervals
void interval_state_handler()
{
  if (interval_state < 14) 
  {
    display.print(intervals[interval_state]);
    display.println(F(" sec"));
  }
  else if (interval_state == 14) {display.println(F("1 min"));}
  else if (interval_state == 15) {display.println(F("1.5 min"));}
  else if (interval_state == 16) {display.println(F("2 min"));}
  else if (interval_state == 17) {display.println(F("2.5 min"));}
  else if (interval_state == 18) {display.println(F("3 min"));}
  else if (interval_state == 19) {display.println(F("4 min"));}
}

//Function to wrap the calculation and display of estimated total time 
void total_time_handler()
{
  //The estimated duration of the time lapse in seconds
  unsigned int total_time = numb_shots * intervals[interval_state];
  //Convert to minutes
  total_time = total_time/60;

  if(total_time <= 60)
  {
    display.print(total_time);
    display.println(" min");
  }
  
  //Convert to hours if needed
  if(total_time > 60)
  {
    total_time = total_time/60;
    display.print(total_time);
    display.println(" hrs");
  }
}

//Fucntion to handle GFX for panning time lapse menu


//Function to enable manual control of slider using buttons
void manual_jog()
{
  if (button1_state == HIGH && button2_state == LOW)
  {
    drive_step(1);
    //temp_step_counter++;
  }
  else if (button2_state == HIGH && button1_state == LOW)
  {
    drive_step(0);
    //temp_step_counter--;
  }

  if (button3_state == HIGH && print_state == HIGH) 
    {
      take_shot(global_shutter_delay);
    }

  //display.clearDisplay();
  //display.setCursor(0,0);
  //display.setTextColor(WHITE);
  //display.setTextSize(2);
  //display.println(" ");
  //display.println(temp_step_counter);
  //display.display();
}

//Function to display potentiometer values with bar graph
void display_pot_values()
{
  byte bar1 = map(pot1_val, 0, 1023, 0, 104);
  byte bar2 = map(pot2_val, 0, 1023, 0, 104);
  byte bar3 = map(pot3_val, 0, 1023, 0, 104);
  byte bar4 = map(pot4_val, 0, 1023, 0, 104);
  byte offset = 8;
  
  display.clearDisplay();
  display.setCursor(0,0);
  display.setTextColor(WHITE);
  display.setTextSize(2);
  display.println(F("POT VALUES"));
  display.setTextSize(1);
  display.println();
  display.print(pot1_val);
  display.fillRect(25, 24, bar1, 7, WHITE);
  display.println();
  display.print(pot2_val);
  display.fillRect(25, 24 + offset, bar2, 7, WHITE);
  display.println();
  display.print(pot3_val);
  display.fillRect(25, 24 + (offset*2), bar3, 7, WHITE);
  display.println();
  display.print(pot4_val);
  display.fillRect(25, 24 + (offset*3), bar4, 7, WHITE);
  display.println();
  display.display();
}

//Function to display four values as four quadrants
void display_data_quadrants(int a, int b, int c, int d)
{
  const byte rad = 4;
  const byte bar_thick = 5;
  byte bar1 = map(pot1_val, 0, 1023, 0, 64);
  byte bar2 = map(pot2_val, 0, 1023, 0, 64);
  byte bar3 = map(pot3_val, 0, 1023, 0, 64);
  byte bar4 = map(pot4_val, 0, 1023, 0, 64);
  
  display.clearDisplay();
  display.setTextColor(WHITE);
  display.setTextSize(1);
  display.setCursor(0,0);
  display.println(" ");
  
  //First quadrant
  display.drawRoundRect(0, 0, 64, 32, rad, WHITE);
  display.fillRect(0, 20, bar1, bar_thick, WHITE);

  //Second quadrant
  display.drawRoundRect(64, 0, 64, 32, rad, WHITE);
  display.fillRect(64, 20, bar2, bar_thick, WHITE);

  //Third quadrant
  display.drawRoundRect(0, 32, 64, 32, rad, WHITE);
  display.fillRect(0, 54, bar3, bar_thick, WHITE);

  //Fourth quadrant
  display.drawRoundRect(64, 32, 64, 32, rad, WHITE);
  display.fillRect(64, 54, bar4, bar_thick, WHITE);

  display.display(); 
}

//LOW-LEVEL MOTOR CONTROL FUNCTIONS -----------------------------------------------
//Function to drive stepper motor forward one step
void drive_step(int dir)
{
  if (dir == 0)
  {
    digitalWrite(dir_pin, LOW);
    digitalWrite(pul_pin, HIGH);
    delayMicroseconds(pulse_delay);
    digitalWrite(pul_pin, LOW);
    delayMicroseconds(pulse_delay);
  }

  else if (dir == 1)
  {
    digitalWrite(dir_pin, HIGH);
    digitalWrite(pul_pin, HIGH);
    delayMicroseconds(pulse_delay);
    digitalWrite(pul_pin, LOW);
    delayMicroseconds(pulse_delay);
  }
}

//Function to move a given number of steps 
//linearly in either direction
//Good value for dly is 350 to 1000
void move_steps(int num_steps, int direct, int dly)
{
  const int ease_steps = 500;
  int temp_counter = ease_steps;
  
  for (int i = 0; i < num_steps; i++)
  {
    //First lets deal with the ease in
    if (i <= ease_steps)
    {
      drive_step(direct);
      temp_counter--;
      delayMicroseconds(dly+temp_counter);
    }
    //Drive linearly
    else if ( i > ease_steps && i < (num_steps-ease_steps) )
    {
      drive_step(direct);
      delayMicroseconds(dly);
    }
    //Now its time for the Ease out
    else if (i > (num_steps-ease_steps) )
    {
      drive_step(direct);
      temp_counter++;
      delayMicroseconds(dly+temp_counter);
    }
  }
}

//LOW-LEVEL CAMERA CONTROL FUNCTIONS ----------------------------------------------

//Function to take a shot
void take_shot(int sh_delay)
{
  digitalWrite(shutter_pin, LOW);
  delay(sh_delay);
  digitalWrite(shutter_pin, HIGH);
}

//Function to focus camera
void focus_shot(int f_delay)
{
  digitalWrite(focus_pin, LOW);
  delay(f_delay);
  digitalWrite(focus_pin, HIGH);
}
