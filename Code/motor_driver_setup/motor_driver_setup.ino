/* Example sketch to control a stepper motor with TB6600 stepper motor driver and Arduino without a library: continuous rotation. More info: https://www.makerguides.com */

#include <Stepper.h>

// Define stepper motor connections:
#define powerPin 3
#define stepGnd 4
#define stepPin 5
#define dirPin 6
#define dirGnd 7


const int stepsPerRevolution = 200;  // change this to fit the number of steps per revolution
// for your motor


// initialize the stepper library on pins 8 through 11:
Stepper myStepper(stepsPerRevolution, stepGnd, stepPin, dirPin, dirGnd);

int stepCount = 0;  // number of steps the motor has taken


void setup() {
  // Declare pins as output:
  pinMode(powerPin, OUTPUT);
  pinMode(stepPin, OUTPUT);
  pinMode(dirPin, OUTPUT);
  pinMode(stepGnd, OUTPUT);
  pinMode(dirGnd, OUTPUT);
  // Set the spinning direction CW/CCW:
  digitalWrite(dirPin, LOW);

  // Set stepGnd and dirGND to GND 
  digitalWrite(stepGnd,LOW);
  digitalWrite(dirGnd,LOW);
  
  digitalWrite(powerPin, HIGH);
}

void loop() {
  myStepper.setSpeed(60);
//  // read the sensor value:
//  int sensorReading = analogRead(A0);
//  // map it to a range from 0 to 100:
//  int motorSpeed = map(sensorReading, 0, 1023, 0, 100);
//  // set the motor speed:
//  if (motorSpeed > 0) {
//    myStepper.setSpeed(motorSpeed);
//    // step 1/100 of a revolution:
//    myStepper.step(stepsPerRevolution / 100);
//    }
}
