#include <Arduino.h>
#include <Encoder.h>

#define CW_lead 31   //leading coil connection for CW rotations
#define CCW_lead 32 //leading coil connection for CCW rotation


Encoder P1_Input(CW_lead, CCW_lead);

int P1_count = 0;
int P1_newcount;

void setup(){
    Serial.begin(9600);
    Serial.println("Initailizing testing...");
}

void loop(){
    P1_newcount = P1_Input.read();
    if (P1_newcount > P1_count){
        Serial.print("Incrementing++\t");
        Serial.println(P1_count);
        P1_count = P1_newcount;
    } else if (P1_newcount < P1_count){
        Serial.print("Decrimenting--\t");
        Serial.println(P1_count);
        P1_count = P1_newcount;
    } else {
        //Serial.println("No movement");
    }
}