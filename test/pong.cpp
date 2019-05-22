//library import
#include <Arduino.h>
#include <LiquidCrystal.h>
#include <AccelStepper.h>
#include <Encoder.h>
#include <pitches.h>
#include <GameConstants.h>
#include <pitches.h>
#include <Audio.h>
#include <Wire.h>
#include <SPI.h>
#include <SD.h>
#include <SerialFlash.h>

//connect lcd
const int rs = 7, en = 8, d4 = 9, d5 = 10, d6 = 11, d7 = 12;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// GUItool: begin automatically generated code
AudioPlaySdWav           playSdWav1;     //xy=163,80
AudioOutputAnalogStereo  dacs1;          //xy=405,61
AudioConnection          patchCord1(playSdWav1, 0, dacs1, 0);
AudioConnection          patchCord2(playSdWav1, 1, dacs1, 1);
// GUItool: end automatically generated code
#define SDCARD_CS_PIN    BUILTIN_SDCARD

// note durations: 4 = quarter note, 8 = eighth note, etc.:
int noteDurations[] = {
  4, 8, 8, 4, 4, 4, 4, 4
};

//connect encoder
Encoder P1_Input(P1_CW_lead, P1_CCW_lead);
Encoder P2_Input(P2_CW_lead, P2_CCW_lead);
int step_int = 30; //encoder sensitivity
//connect steppers
//1/4 stepping w/ 40 tooth pulleys == 10 step/mm
int step_mm = 10;
//paddle motors
AccelStepper p1_stepper(1, p1_STEP_PIN, p1_DIR_PIN); //set mode to DRIVER, STEP pin 6, DIR pin 7
AccelStepper p2_stepper(1, p2_STEP_PIN, p2_DIR_PIN); //set mode to DRIVER, STEP pin 6, DIR pin 7
//ball motors
AccelStepper ball_y1_stepper(1, y1_STEP_PIN, y1_DIR_PIN); //set mode to DRIVER, STEP pin 6, DIR pin 7
AccelStepper ball_y2_stepper(1, y2_STEP_PIN, y2_DIR_PIN); //set mode to DRIVER, STEP pin 6, DIR pin 7
AccelStepper ball_x_stepper(1, x_STEP_PIN, x_DIR_PIN); //set mode to DRIVER, STEP pin 6, DIR pin 7

//CORRECT THESE VALUES (just placeholders)*****!!!!
int p1_upperEdge;
int p1_middle;
int p1_lowerEdge;
int p1_lowerThird;
int p1_upperThird;
int p2_upperEdge;
int p2_middle;
int p2_lowerEdge;
int p2_lowerThird;
int p2_upperThird;
int ball_p2_edge;
int ball_p1_edge;
int ball_upperEdge;
int ball_lowerEdge;
int const pxEdge_toZero = step_mm * 32; //measurment for setting goallines
int const y_upperLim = 8000; //8000 // 8500(ylen) - 300(limswitch to edge) - 200(paddle lower edge) steps
int const y_lowerLim = 200; //200
int const y_center = y_upperLim / 2;
int const x_upperLim = 8180; //8180
int const x_lowerLim = 220; //220
int const x_center = x_upperLim / 2;
int const x_p2Goal = x_upperLim - pxEdge_toZero;
int const x_p1Goal = x_upperLim - pxEdge_toZero;


//for edge tracking of paddle and ball
int paddle_len = step_mm * 95; //step/mm * mm == steps
int pyEdge_toZero = step_mm * 19; //step/mm * mm == steps
int ball_width = step_mm * 45; //step/mm * mm == steps
int bxEdge_toZero = step_mm * 18;
int byEdge_toZero = step_mm * 7;

//homing step counter
int p1_homing;
int p2_homing;
int y1_homing;
int y2_homing;
int x_homing;
//encoder input
int P1_input;
int P2_input;
int P1_count = 0;
int P1_steps = 4000; //set to start position
int P2_count = 0;
int P2_steps = 4000; //set to start position

//score variables
int p1_score = 0;
int p2_score = 0;

void reset_ball(){
    ball_x_stepper.moveTo(x_center);
    while (ball_x_stepper.distanceToGo() != 0){
        ball_x_stepper.run();
    }
    ball_y1_stepper.moveTo(y_center);
    ball_y2_stepper.moveTo(y_center);
    while (ball_y1_stepper.distanceToGo() != 0 && ball_y2_stepper.distanceToGo() != 0){
        ball_y1_stepper.run();
        ball_y2_stepper.run();
    }
}

void update_score(){
    lcd.setCursor(0,1);
    lcd.print("P1:");
    lcd.setCursor(4,1);
    lcd.print(p1_score);
    lcd.setCursor(11,1);
    lcd.print("P2:");
    lcd.setCursor(14,1);
    lcd.print(p2_score);
}

void p1_goal(){
    //what to do when p1 scores
    //have switch cases for game modes
    digitalWrite(P1_goalPin, HIGH);
    //delay(100); //add if necessary
    digitalWrite(P1_goalPin, LOW);
    p1_score++;
    update_score();
    reset_ball();
}

void p2_goal(){
    //what to do when p2 scores
    digitalWrite(P2_goalPin, HIGH);
    digitalWrite(P2_goalPin, LOW);
    p2_score++;
    update_score();
    reset_ball();
}

void ball_y_movement(){
    if (ball_y1_stepper.currentPosition() >= y_upperLim && ball_y2_stepper.currentPosition() >= y_upperLim){
        ball_y1_stepper.moveTo(y_lowerLim);
        ball_y2_stepper.moveTo(y_lowerLim);
        playSdWav1.play("BOUNCE.wav");
    } else if (ball_y1_stepper.currentPosition() <= y_lowerLim && ball_y2_stepper.currentPosition() <= y_lowerLim){
        ball_y1_stepper.moveTo(y_upperLim);
        ball_y2_stepper.moveTo(y_upperLim);
    }
    ball_y1_stepper.run();
    ball_y2_stepper.run();
    ball_upperEdge = ball_y2_stepper.currentPosition() + byEdge_toZero + ball_width;
    ball_lowerEdge = ball_y2_stepper.currentPosition() + byEdge_toZero;
}

void ball_x_movement(){
    //detect paddle hits or goal
    //p1
    if (ball_p1_edge == x_p1Goal && (ball_lowerEdge < p1_upperEdge || ball_upperEdge > p1_lowerEdge)){
        ball_x_stepper.moveTo(x_upperLim);
        //redirect hits
        if (ball_lowerEdge > p1_upperThird){
            ball_y1_stepper.moveTo(y_upperLim);
            ball_y2_stepper.moveTo(y_upperLim);
        } else if (ball_upperEdge < p1_lowerThird){
            ball_y1_stepper.moveTo(y_lowerLim);
            ball_y2_stepper.moveTo(y_lowerLim);
        }
        playSdWav1.play("BALLHIT1.wav");
    } else if (ball_p1_edge > x_p1Goal){
        p1_stepper.stop();
        p2_goal();
    }
    //p2
    if (ball_p2_edge == x_p2Goal && (ball_lowerEdge < p2_upperEdge || ball_upperEdge > p2_lowerEdge)){
        ball_x_stepper.moveTo(x_lowerLim);
        //redirect hits
        if (ball_lowerEdge > p2_upperThird){
            ball_y1_stepper.moveTo(y_upperLim);
            ball_y2_stepper.moveTo(y_upperLim);
        } else if (ball_upperEdge < p2_lowerThird){
            ball_y1_stepper.moveTo(y_lowerLim);
            ball_y2_stepper.moveTo(y_lowerLim);
        }
        playSdWav1.play("BALLHIT.wav");
    } else if (ball_p2_edge > x_p2Goal){
        p2_stepper.stop();
        p1_goal();
    }
    ball_p1_edge = ball_x_stepper.currentPosition() + bxEdge_toZero;
    ball_p2_edge = ball_x_stepper.currentPosition() + bxEdge_toZero + ball_width;
}

void ball(){
    //call ball x movement
    ball_x_movement();
    //call ball y movement
    ball_y_movement();
}

void p1_paddle(){
    //have switch cases for modes
    P1_input = P1_Input.read();
    if (P1_input > P1_count){
        P1_count = P1_input;
        if (P1_steps > y_upperLim){
            P1_steps = P1_steps - step_int; //if this is too slow increase by multiple P1_steps like 5 or 10
            p1_stepper.moveTo(P1_steps);
        }
    } else if (P1_input < P1_count){
        P1_count = P1_input;
        if (P1_steps < y_lowerLim){
            P1_steps = P1_steps + step_int;
            p1_stepper.moveTo(P1_steps);
        }
    }
    p1_upperEdge = p1_stepper.currentPosition() + pyEdge_toZero + paddle_len;
    p1_lowerEdge = p1_stepper.currentPosition() + pyEdge_toZero;
    p1_lowerThird = p1_lowerEdge + 30;
    p1_upperThird = p1_upperEdge - 30;
}

void p2_paddle(){
    //have switch cases for modes
    P2_input = P2_Input.read();
    if (P2_input > P2_count){
        P2_count = P2_input;
        if (P2_steps > y_upperLim){
            P2_steps = P2_steps - step_int; //if this is too slow increase by multiple P2_steps like 5 or 10
            p2_stepper.moveTo(P1_steps);
        }
    } else if (P2_input < P2_count){
        P2_count = P2_input;
        if (P2_steps < y_lowerLim){
            P2_steps = P2_steps + step_int;
            p2_stepper.moveTo(P2_steps);
        }
    }
    p2_upperEdge = p2_stepper.currentPosition() + pyEdge_toZero + paddle_len;
    p2_lowerEdge = p2_stepper.currentPosition() + pyEdge_toZero;
    p2_lowerThird = p2_lowerEdge + 30;
    p2_upperThird = p2_upperEdge - 30;
}

void homing(){
    pinMode(p1_homePin, INPUT_PULLUP);
    pinMode(p2_homePin, INPUT_PULLUP);
    pinMode(y1_homePin, INPUT_PULLUP);
    pinMode(y2_homePin, INPUT_PULLUP);
    pinMode(x_homePin, INPUT_PULLUP);
    //home paddles first 
    //set stepper homing speed
    //p1
    p1_stepper.setCurrentPosition(0);
    p1_stepper.setPinsInverted(true);
    p1_stepper.setMaxSpeed(2000);
    p1_stepper.setAcceleration(2000);
    p1_homing = 1;
    while (digitalRead(p1_homePin)){
        p1_stepper.moveTo(p1_homing);
        p1_stepper.run();
        p1_homing--; //depends on motor direction
        delay(5);
    }
    p1_stepper.setCurrentPosition(0);
    p1_stepper.moveTo(y_center); //send paddles to center
    while (p1_stepper.distanceToGo() != 0){
        p1_stepper.run();
    }
    //p2
    p2_stepper.setCurrentPosition(0);
    p2_stepper.setMaxSpeed(2000);
    p2_stepper.setAcceleration(2000);
    p2_homing = 1;
    while (digitalRead(p2_homePin)){
        p2_stepper.moveTo(p2_homing);
        p2_stepper.run();
        p2_homing--; //depends on motor direction
        delay(5);
    }
    p2_stepper.setCurrentPosition(0);
    p2_stepper.moveTo(y_center); //send paddles to center
    while (p2_stepper.distanceToGo() != 0){
        p2_stepper.run();
    }

    //home ball y
    ball_y1_stepper.setPinsInverted(true);
    ball_y1_stepper.setCurrentPosition(0);
    ball_y1_stepper.setMaxSpeed(2000);
    ball_y1_stepper.setAcceleration(2000);
    ball_y2_stepper.setPinsInverted(true);
    ball_y2_stepper.setCurrentPosition(0);
    ball_y2_stepper.setMaxSpeed(2000);
    ball_y2_stepper.setAcceleration(2000);
    y1_homing = 1;
    y2_homing = 1;
    while (digitalRead(y1_homePin) || digitalRead(y2_homePin)){
        if (digitalRead(y1_homePin) == HIGH){
            ball_y1_stepper.moveTo(y1_homing);
            ball_y1_stepper.run();
            y1_homing--;
            delay(5);
        }
        if (digitalRead(y2_homePin) == HIGH){
            ball_y2_stepper.moveTo(y2_homing);
            ball_y2_stepper.run();
            y2_homing--;
            delay(5);
        }
    }
    ball_y1_stepper.setCurrentPosition(0);
    ball_y2_stepper.setCurrentPosition(0);

    ball_y1_stepper.moveTo(y_lowerLim);
    ball_y2_stepper.moveTo(y_lowerLim);
    while ((ball_y1_stepper.distanceToGo() != 0) || (ball_y2_stepper.distanceToGo() != 0)){
        ball_y1_stepper.run();
        ball_y2_stepper.run();
    }
    //home ball x
    ball_x_stepper.setCurrentPosition(0);
    ball_x_stepper.setMaxSpeed(2000);
    ball_x_stepper.setAcceleration(2000);
    x_homing = 1;
    while (digitalRead(x_homePin)){
        ball_x_stepper.moveTo(x_homing);
        ball_x_stepper.run();
        x_homing--; //depends on motor direction
        delay(5);
    }
    ball_x_stepper.setCurrentPosition(0);
    //send ball to center
    ball_x_stepper.moveTo(x_center); //send ball x to center
    while (ball_x_stepper.distanceToGo() != 0){
        ball_x_stepper.run();
    }
    ball_y1_stepper.moveTo(y_center); //send call to y center
    ball_y2_stepper.moveTo(y_center);
    while (ball_y1_stepper.distanceToGo() != 0 && ball_y2_stepper.distanceToGo() != 0){
        ball_y1_stepper.run();
        ball_y2_stepper.run();
    }
    playSdWav1.play("INITIALIZATION.wav");
}

void setup(){
    //pic communication
    pinMode(ledPin, OUTPUT);
    pinMode(P1_goalPin, OUTPUT);
    pinMode(P2_goalPin, OUTPUT);
    pinMode(p1_hit, OUTPUT);
    pinMode(p2_hit, OUTPUT);
    pinMode(Blue_ON, OUTPUT);
    pinMode(Green_ON, OUTPUT);
    digitalWrite(ledPin, HIGH);

    AudioMemory(8); //set memory for sd card access

    // iterate over the notes of the melody:
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
    
    digitalWrite(Blue_ON, HIGH);
    lcd.begin(16,2);
    lcd.print("Mechanical Pong");
    lcd.setCursor(0,1);
    lcd.print("Initializing...");
    digitalWrite(Blue_ON, LOW);

    //home steppers
    homing();
    lcd.print(">>>Completed<<<");
    digitalWrite(Green_ON, HIGH);
    delay(2000);
    digitalWrite(Green_ON, LOW);

    lcd.clear();
    
    //print player scores
    update_score();

    //Game speeds
    ball_y1_stepper.setMaxSpeed(5000);
    ball_y1_stepper.setAcceleration(46000);
    ball_y2_stepper.setMaxSpeed(5000);
    ball_y2_stepper.setAcceleration(46000);
    ball_x_stepper.setMaxSpeed(5000);
    ball_x_stepper.setAcceleration(46000);
    p1_stepper.setMaxSpeed(5000);
    p1_stepper.setAcceleration(16000);
    p2_stepper.setMaxSpeed(5000);
    p2_stepper.setAcceleration(16000);

}

void loop(){
    if (digitalRead(selectButton) == HIGH && digitalRead(nextButton) == HIGH){
        p1_score = 0;
        p2_score = 0;
        update_score();
        reset_ball();
    }
    //call ball()
    ball();
    //call p1_paddle()
    p1_paddle();
    //call p2_paddle()
    p2_paddle();
}