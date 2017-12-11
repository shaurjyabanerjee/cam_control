//need to limit rotatation (-100, 100) from position reset when setting to auto

#include <Wire.h>
#include <Adafruit_MotorShield.h>
#include "utility/Adafruit_MS_PWMServoDriver.h"

///////////////////////////
//PINS/////////////////////
///////////////////////////

const int tiltLeftPin = 4;
const int tiltRightPin = 5;

const int panLeftPin = 7;
const int panRightPin = 6;

const int manualPin = 8;
const int autoPin = 9;

const int cyclePin = 10;

const int speedPin = A0;

const int stepType = MICROSTEP;

int step_delay = 0;

///////////////////////////
//CONTROL//////////////////
///////////////////////////

int tiltLeftState;
int tiltRightState;

int panLeftState;
int panRightState;

int manualState;
int autoState;

int cycleState;
int lastCycle;

boolean controlsOn;

int speedVal = 1;
float maxSpeed = 10.0;

///////////////////////////
//MOTORS///////////////////
///////////////////////////

Adafruit_MotorShield AFMS = Adafruit_MotorShield(0x60);
Adafruit_MotorShield AFMS2 = Adafruit_MotorShield(0x61);
Adafruit_StepperMotor *tiltMotor = AFMS.getStepper(200, 2);
Adafruit_StepperMotor *panMotor = AFMS2.getStepper(200, 2);

///////////////////////////
//CYCLE////////////////////
///////////////////////////

const int MAX_TILT_STEPS = 300;
const int MAX_PAN_STEPS = 100;

boolean cycleOn = false;

int numPositions = 4;

int tiltOffsets[] = {300, -150, -150, -300};
int panOffsets[] = {100, -50, -50, -100};
int tiltSteps = 0;
int panSteps = 0;

int tiltOrigin = 0;
int panOrigin = 0;

int tiltTarget = 0;
int panTarget = 0;

int delays[] = {1000, 1000, 500, 2000};

int index = 0;

void setup() {
  Serial.begin(9600);

  pinMode(tiltLeftPin, INPUT);
  pinMode(tiltRightPin, INPUT);

  pinMode(panLeftPin, INPUT);
  pinMode(panRightPin, INPUT);

  pinMode(manualState, INPUT);
  pinMode(autoState, INPUT);

  pinMode(cycleState, INPUT);

  AFMS.begin();
  AFMS2.begin();
  tiltMotor->setSpeed(0);
  panMotor->setSpeed(0);
}

void loop() {
  //handleSpeed();

  handleControlState();
  handleCycleStart();

  if (controlsOn) {
    cycleOn = false;

    handleTiltButtons();
    handlePanButtons();
  }

  if (autoState == HIGH) {
    if (cycleOn) {
      runCycle();
    } else {

      tiltOrigin = 0;
      panOrigin = 0;

      index = 0;
      tiltMotor->setSpeed(0);
      panMotor->setSpeed(0);
    }
  }
   
  //printInput();
}

void printInput() {
  delay(10);

  Serial.println("///////////////////////////");

  Serial.print("tilt left: ");
  Serial.print(tiltLeftState);
  Serial.println();

  Serial.print("tilt right: ");
  Serial.print(tiltRightState);
  Serial.println();

  Serial.print("pan left: ");
  Serial.print(panLeftState);
  Serial.println();

  Serial.print("pan right: ");
  Serial.print(panRightState);
  Serial.println();

  Serial.print("manual control: ");
  Serial.print(manualState);
  Serial.println();

  Serial.print("auto cycle: ");
  Serial.print(autoState);
  Serial.println();

  Serial.print("cycle start: ");
  Serial.print(cycleState);
  Serial.println();

  Serial.print("speed: ");
  Serial.print(speedVal);
  Serial.println();

  Serial.println("///////////////////////////");
}

void handleTiltButtons() {
  tiltLeftState = digitalRead(tiltLeftPin);
  tiltRightState = digitalRead(tiltRightPin);
  step_delay = analogRead(speedPin);

  if (tiltLeftState == HIGH && tiltRightState == LOW) {
    tiltMotor->setSpeed(speedVal);
    tiltMotor->step(1, BACKWARD, stepType);
    tiltMotor->setSpeed(0);
  }

  if (tiltRightState == HIGH && tiltLeftState == LOW) {
    tiltMotor->setSpeed(speedVal);
    tiltMotor->step(1, FORWARD, stepType);
    tiltMotor->setSpeed(0);
  }
  
  delay(step_delay/4);
}

void handlePanButtons() {
  panLeftState = digitalRead(panLeftPin);
  panRightState = digitalRead(panRightPin);
  step_delay = analogRead(speedPin);

  if (panLeftState == HIGH && panRightState == LOW) {
    panMotor->setSpeed(speedVal);
    panMotor->step(1, BACKWARD, stepType);
    panMotor->setSpeed(0);
  }

  if (panRightState == HIGH && panLeftState == LOW) {
    panMotor->setSpeed(speedVal);
    panMotor->step(1, FORWARD, stepType);
    panMotor->setSpeed(0);
  }
  
  delay(step_delay);
}

void handleControlState() {
  manualState = digitalRead(manualPin);
  autoState = digitalRead(autoPin);

  if (manualState == HIGH && autoState == LOW) {
    controlsOn = true;
  }

  if (manualState == LOW && autoState == HIGH) {
    controlsOn = false;
  }
  
  delay(step_delay/4);
}

void handleCycleStart() {
  cycleState = digitalRead(cyclePin);

  if (cycleState == HIGH && lastCycle == LOW) {
    cycleOn = !cycleOn;
  }

  lastCycle = cycleState;
}

void handleSpeed() {
  speedVal = analogRead(speedPin);

  float s = float(speedVal) / 1024;
  s *= maxSpeed;
  speedVal = int(s);
}

void runCycle() {

  if (tiltSteps < abs(tiltOffsets[index])) {

    if (tiltOffsets[index] < 0) {

      tiltMotor->setSpeed(speedVal);
      tiltMotor->step(1, BACKWARD, stepType);
      tiltMotor->setSpeed(0);

      tiltOrigin--;

    } else if (tiltOffsets[index] > 0) {

      tiltMotor->setSpeed(speedVal);
      tiltMotor->step(1, FORWARD, stepType);
      tiltMotor->setSpeed(0);

      tiltOrigin++;

    } else {
      tiltMotor->setSpeed(0);
    }
    
    tiltSteps++;
  }

  if (panSteps < abs(panOffsets[index])) {

    if (panOffsets[index] < 0) {

      panMotor->setSpeed(speedVal);
      panMotor->step(1, BACKWARD, stepType);
      panMotor->setSpeed(0);

      panOrigin--;

    } else if (panOffsets[index] > 0) {

      panMotor->setSpeed(speedVal);
      panMotor->step(1, FORWARD, stepType);
      panMotor->setSpeed(0);

      panOrigin++;
    } else {
      panMotor->setSpeed(0);
    }
    
    panSteps++;
  }

  if (tiltSteps >= abs(tiltOffsets[index]) && panSteps >= abs(panOffsets[index])) {
    
    tiltTarget = tiltOffsets[index] + tiltOrigin;
    
    if (tiltOffsets[index] > 0){
      int tiltOverflow = tiltTarget - MAX_TILT_STEPS;
      tiltTarget -= tiltOverflow;   
    } else if (tiltOffsets[index] < 0){
      int tiltOverflow = tiltTarget + MAX_TILT_STEPS;
      tiltTarget -= tiltOverflow;
    }
    
    Serial.println(tiltOrigin);
    Serial.println(panOrigin);
    
    tiltSteps = 0;
    panSteps = 0;

    Serial.println(index);
    delay(delays[index]);

    index++;
  }

  if (index > numPositions - 1) {
    index = 0;
  }
}
