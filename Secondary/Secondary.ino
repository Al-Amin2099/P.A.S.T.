
#include <AccelStepper.h>
#include <Wire.h>

#define HALFSTEP 8

// Motor Pins
#define pin1 2 // IN1 on ULN2003
#define pin2 3 // IN2 on ULN2003
#define pin3 4 // IN3 on ULN2003
#define pin4 5 // IN4 on ULN2003

#define pin5 A0 // IN1 on ULN2003
#define pin6 A1 // IN2 on ULN2003
#define pin7 A2 // IN3 on ULN2003
#define pin8 A3 // IN4 on ULN2003

// Initializing pin sequence: I1-In3-In2-In4 for using AccelStepper with 28BYJ-48
AccelStepper stepper1(HALFSTEP, pin1, pin3, pin2, pin4);
AccelStepper stepper2(HALFSTEP, pin5, pin7, pin6, pin8);

int stahp = 7;
int stahp2 = 10;
int cw = 6;
int cw2 = 11;
int ccw = 8;
int cc2 = 9;

bool stopped = false;
bool stopped2 = false;

void setup() 
{
  stepper1.setMaxSpeed(1000.0);
  stepper2.setMaxSpeed(1000.0);

  pinMode(stahp2, INPUT);
  pinMode(cw2, INPUT);
  pinMode(ccw2, INPUT);
  pinMode(stahp, INPUT);
  pinMode(cw, INPUT);
  pinMode(ccw, INPUT);
}

void loop() 
{
  motorPitch();
  motorRoll();

  if(stopped == false)
  {
    stepper1.run();
  }
  
  if(stopped2 == false)
  {
    stepper2.run();
  }
}

void motorPitch()
{
  if(digitalRead(stahp2) == HIGH)
  { 
    stopped2 = true;
  }
  else if(digitalRead(cw2) == HIGH)
  {
    stepper2.setSpeed(100);
    stopped2 = false;
  }

  if(digitalRead(ccw2) == HIGH)
  {
    stepper2.setSpeed(-100)
    stopped2 = false;
  }
}

void motorRoll()
{
  if(digitalRead(stahp) == HIGH)
  {
    stopped = true;
  }
  else if(digitalRead(cw) == HIGH)
  {
    stepper1.setSpeed(100);
    stopped = false;
  }

  if(digitalRead(ccw) == HIGH)
  {
    stepper1.setSpeed(-100);
    stopped = false;
  }
}
