#include <Arduino.h>

#define audioPin 35

void setup(){

}

void loop(){
    tone(audioPin, 528, 500);
}