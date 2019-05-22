#include <Arduino.h>
#include <LiquidCrystal.h>
#include <AccelStepper.h>
#include <Encoder.h>
#include <pitches.h>

//encoder pins
#define CW_lead 31   //leading coil connection for CW rotations
#define CCW_lead 32 //leading coil connection for CCW rotation
//stepper pins
#define stepPin 23
#define dirPin 22
//limit switch
#define homePin 21
//audio output
#define audioPin 35
//goal pins
#define P1_goal 1
#define P2_goal 0

//connect lcd
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

//connect encoder
Encoder P1_Input(CW_lead, CCW_lead);
//connect stepper
AccelStepper stepper(1, stepPin, dirPin); //set mode to DRIVER, STEP pin 6, DIR pin 7
int initial_homing;
int lower_limit = 7000;
int upper_limit = 400;
int step_int = 30; //encoder sensitivity
int P1_input;
int P1_count = 0;
int P1_steps = 3300; //set to start position

//for sound
// notes in the melody:
int melody[] = {
  NOTE_C4, NOTE_G3, NOTE_G3, NOTE_A3, NOTE_G3, 0, NOTE_B3, NOTE_C4
};
// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

void setup(){
    delay(1000); //delay for boot up of system.
    Serial.begin(9600);
    //pic communication
    pinMode(P1_goal, OUTPUT);
    pinMode(P2_goal, OUTPUT);
    //connect lcd
    lcd.begin(16, 2);
    delay(100);
    //lcd print welcome
    lcd.print("Mechancial Pong");
    lcd.setCursor(0,1); //move cursor down to second line
    lcd.print("Demo!");
    delay(2000);
    lcd.clear();
    lcd.print("Homing stepper");
    lcd.setCursor(0,1);
    delay(2000);

    pinMode(homePin, INPUT_PULLUP);
    //set stepper homing speed
    stepper.setCurrentPosition(0);
    stepper.setMaxSpeed(2000);
    stepper.setAcceleration(2000);
    initial_homing = 1;
    while (digitalRead(homePin)){
        stepper.moveTo(initial_homing);
        stepper.run();
        initial_homing--;
        delay(5);
    }
    stepper.setCurrentPosition(0);
    delay(1000);

    //move stepper away from limit switch give safety factor
    stepper.moveTo(P1_steps);
    while (stepper.distanceToGo() != 0){
        stepper.run();
    }
    lcd.print("*Completed*");
    digitalWrite(P1_goal, HIGH);
    delay(100);
    digitalWrite(P1_goal, LOW);

    //Play melody to signify start of game
    for (int thisNote = 0; thisNote < 8; thisNote++) {
        // to calculate the note duration, take one second divided by the note type.
        //e.g. quarter note = 1000 / 4, eighth note = 1000/8, etc.
        int noteDuration = 1000 / noteDurations[thisNote];
        tone(35, melody[thisNote], noteDuration);
        // to distinguish the notes, set a minimum time between them.
        // the note's duration + 30% seems to work well:
        int pauseBetweenNotes = noteDuration * 1.30;
        delay(pauseBetweenNotes);
        // stop the tone playing:
        noTone(8);
  }
    digitalWrite(P2_goal, HIGH);
    delay(100);
    digitalWrite(P2_goal, LOW);
    lcd.clear();
    //stepper settings for playing
    stepper.setMaxSpeed(5000);  //higher = faster
    stepper.setAcceleration(16000); //higher = faster
    lcd.print("Begin round!");
}

void loop(){
    P1_input = P1_Input.read();
    if (P1_input > P1_count){
        P1_count = P1_input;
        if (P1_steps > upper_limit){
            P1_steps = P1_steps - step_int; //if this is too slow increase by multiple P1_steps like 5 or 10
            stepper.moveTo(P1_steps);
        }
    } else if (P1_input < P1_count){
        P1_count = P1_input;
        if (P1_steps < lower_limit){
            P1_steps = P1_steps + step_int;
            stepper.moveTo(P1_steps);
        }
    }
    //Serial.print(stepper.currentPosition());
    //Serial.print("\t");
    //Serial.println(P1_steps);
    stepper.run();
}