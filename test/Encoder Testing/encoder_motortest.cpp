#include <Arduino.h>
#include <Encoder.h>
#include <AccelStepper.h>

#define CW_lead 31   //leading coil connection for CW rotations
#define CCW_lead 32 //leading coil connection for CCW rotation
#define stepPin 6
#define dirPin 7


Encoder P1_Input(CW_lead, CCW_lead);
AccelStepper stepper(1, stepPin, dirPin); //set mode to DRIVER, STEP pin 6, DIR pin 7

int lower_limit = 0;
int upper_limit = 800;
int P1_count = 0;
int P1_newcount;
int dir = 0; //0 = no movement, 1 = CW, 2 = CCW

void setup(){
    Serial.begin(9600);
    stepper.setMaxSpeed(1000);  //higher = faster
    stepper.setAcceleration(6000); //higher = faster
    stepper.setMinPulseWidth(1000); //weird steppers
    Serial.println("Initailizing testing...");
}

void loop(){
    P1_newcount = P1_Input.read();
    if (P1_newcount > P1_count){
        dir = 1;
    } else if (P1_newcount < P1_count){
        dir = 2;
    } else {
        dir = 0;
    }
    switch (dir){
        case 0:
            Serial.println("No movement");
            stepper.move(0);
            break;
        case 1:
            Serial.print("Incrementing++\t");
            Serial.println(P1_count);
            stepper.moveTo(upper_limit);
            P1_count = P1_newcount;
            break;
        case 2:
            Serial.print("Decrimenting--\t");
            Serial.println(P1_count);
            stepper.moveTo(lower_limit);
            P1_count = P1_newcount;
            break;
    }
    stepper.run();
}

// void loop(){
//     P1_newcount = P1_Input.read();
//     if (P1_newcount > P1_count){
//         Serial.print("Incrementing++\t");
//         Serial.println(P1_count);
//         stepper.moveTo(upper_limit);
//         P1_count = P1_newcount;
//     } else if (P1_newcount < P1_count){
//         Serial.print("Decrimenting--\t");
//         Serial.println(P1_count);
//         stepper.moveTo(lower_limit);
//         P1_count = P1_newcount;
//     } else {
//         Serial.println("No movement");
//         stepper.move(0);
//     }
//     stepper.run();
// }