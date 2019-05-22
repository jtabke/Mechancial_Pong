//VIDEO LINK: https://youtu.be/uxEHUWYzTUI
#include <Arduino.h>
#include <AccelStepper.h>

#define stepPin 6
#define dirPin 7
#define ANALOG_IN A0


AccelStepper stepper(1, stepPin, dirPin); //set mode to DRIVER, STEP pin 6, DIR pin 7

int steps = 0;
int analog_in =0;
int counter = 0;
int lower_limit = 0;
int upper_limit = 800;
#define count_lim 100

void setup() {  
  //Serial.begin(9600);
  stepper.setMaxSpeed(1000);  //higher = faster
  stepper.setAcceleration(6000); //higher = faster
  stepper.setMinPulseWidth(1000); //weird steppers
}


void loop() {
  //move the motor toward the upper or lower limit baed on the joystick direction
  switch (analog_in){
  case 1023:
    stepper.moveTo(upper_limit);
    stepper.run();
    break;
  case 0:
    stepper.moveTo(lower_limit);
    stepper.run();
    break;
  default:
    stepper.move(0);
    break;
}
  //Used to decrease the amount of anlogRead's
  switch (counter){
    case count_lim:
      analog_in = analogRead(ANALOG_IN);
      counter = 0;
      break;
  }

  stepper.run();
  counter++;
  //for debugging
  //Serial.print(analog_in);
  //Serial.print('\t');
  //Serial.println(stepper.currentPosition());
}