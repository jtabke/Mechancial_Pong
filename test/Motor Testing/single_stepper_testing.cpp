#include <Arduino.h>
#include <AccelStepper.h>

#define stepPin 23
#define dirPin 22

#define Steps 400 
AccelStepper stepper(1, stepPin, dirPin); //set mode to DRIVER, STEP pin 6, DIR pin 7

void setup(){
  // Change these to suit your stepper if you want
  stepper.setMaxSpeed(1000); //higher = faster
  stepper.setAcceleration(2000); //higher = faster
  stepper.moveTo(1200);
  stepper.setMinPulseWidth(1000);
}

void loop(){
    // If at the end of travel go to the other end
    if (stepper.distanceToGo() == 0)
      stepper.moveTo(-stepper.currentPosition());
    stepper.run();
}