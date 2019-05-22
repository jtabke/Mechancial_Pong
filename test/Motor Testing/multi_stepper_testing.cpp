#include <Arduino.h>
#include <AccelStepper.h>

#define stepPin1 6
#define dirPin1 7
#define stepPin2 4
#define dirPin2 5
#define stepPin3 2
#define dirPin3 3

#define Steps 400 
AccelStepper stepper1(1, stepPin1, dirPin1); //set mode to DRIVER, STEP pin 6, DIR pin 7
AccelStepper stepper2(1, stepPin2, dirPin2); //set mode to DRIVER, STEP pin 4, DIR pin 5
AccelStepper stepper3(1, stepPin3, dirPin3); //set mode to DRIVER, STEP pin 2, DIR pin 3

void setup(){
  // Change these to suit your stepper if you want
  stepper1.setMaxSpeed(300);
  stepper1.setAcceleration(300);
  stepper1.moveTo(900);
  stepper2.setMaxSpeed(300);
  stepper2.setAcceleration(300);
  stepper2.moveTo(900);
  stepper3.setMaxSpeed(300);
  stepper3.setAcceleration(300);
  stepper3.moveTo(900);
}

void loop(){
    // If at the end of travel go to the other end
    if (stepper1.distanceToGo() == 0)
      stepper1.moveTo(-stepper1.currentPosition());
    if (stepper2.distanceToGo() == 0)
      stepper2.moveTo(-stepper2.currentPosition());
    if (stepper3.distanceToGo() == 0)
      stepper3.moveTo(-stepper3.currentPosition());
    stepper1.run();
    stepper2.run();
    stepper3.run();
}