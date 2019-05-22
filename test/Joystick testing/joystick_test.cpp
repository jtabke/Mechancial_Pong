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
int upper_limit = 2400;
void setup()
{  
  stepper.setMaxSpeed(1000);
  stepper.setAcceleration(2000); //higher = faster
  stepper.setMinPulseWidth(1000);
}
void loop()
{
  // Read new position
  if (counter == 1000){
    analog_in = analogRead(ANALOG_IN);
  }
  
  if (analog_in > 512 + 10){
    stepper.moveTo(upper_limit);
    stepper.run();
  } else if (analog_in < 512 - 10){
    stepper.moveTo(lower_limit);
    stepper.run();
  }

  if (analog_in < 512 + 10 &&  analog_in > 512 - 10){
    stepper.stop();
  }
  counter++;
}