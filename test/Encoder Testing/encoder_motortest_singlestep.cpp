#include <Arduino.h>
#include <AccelStepper.h>
#include <Encoder.h>

//encoder pins
#define CW_lead 25   //leading coil connection for CW rotations
#define CCW_lead 24 //leading coil connection for CCW rotation
//stepper pins
#define stepPin 23
#define dirPin 22

//connect encoder
Encoder P1_Input(CW_lead, CCW_lead);
//connect stepper
AccelStepper stepper(1, stepPin, dirPin); //set mode to DRIVER, STEP pin 6, DIR pin 7
int step_int = 15; //encoder sensitivity
int P1_input;
int P1_count = 0;
int P1_steps = 0; //set to start position


void setup(){
    delay(1000); //delay for boot up of system.
    Serial.begin(9600);
    stepper.setMaxSpeed(1000);  //higher = faster
    stepper.setAcceleration(2000); //higher = faster
}

void loop(){
    P1_input = P1_Input.read();
    if (P1_input > P1_count){
        P1_count = P1_input;
        P1_steps = P1_steps - step_int; //if this is too slow increase by multiple P1_steps like 5 or 10
        stepper.moveTo(P1_steps);
        }
    else if (P1_input < P1_count){
        P1_count = P1_input;
        P1_steps = P1_steps + step_int;
        stepper.moveTo(P1_steps);
        }
    //Serial.print(stepper.currentPosition());
    //Serial.print("\t");
    //Serial.println(P1_steps);
    stepper.run();
}