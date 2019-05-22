#include <Arduino.h>
#include <AccelStepper.h>

#define x1_STEP_PIN 1//PUT PIN NUM!!!***
#define x1_DIR_PIN 2//PUT PIN NUM!!!***

#define x2_STEP_PIN 3//PUT PIN NUM!!!***
#define x2_DIR_PIN 4//PUT PIN NUM!!!***

#define y_STEP_PIN 5//PUT PIN NUM!!!***
#define y_DIR_PIN 6//PUT PIN NUM!!!***

AccelStepper ball_x1_stepper(1, x1_STEP_PIN, x1_DIR_PIN); //set mode to DRIVER, STEP pin 6, DIR pin 7
AccelStepper ball_x2_stepper(1, x2_STEP_PIN, x2_DIR_PIN); //set mode to DRIVER, STEP pin 6, DIR pin 7
AccelStepper ball_y_stepper(1, y_STEP_PIN, y_DIR_PIN); //set mode to DRIVER, STEP pin 6, DIR pin 7


//CORRECT THESE VALUES (just placeholders)
int const y_upperLim = 2200;
int const y_lowerLim = 200;
int const x_rightPadEdge = 4600;
int const x_leftPadEdge = 200;

void setup() {
    Serial.begin(9600);
    ball_x1_stepper.setMaxSpeed(1000);
    ball_x1_stepper.setAcceleration(2000);
    ball_x2_stepper.setMaxSpeed(1000);
    ball_x2_stepper.setAcceleration(2000);
    ball_y_stepper.setMaxSpeed(1000);
    ball_y_stepper.setAcceleration(2000);
}

void ball_y_movement() {
    switch (ball_y_stepper.currentPosition())
    {
        case y_upperLim:
            ball_y_stepper.moveTo(y_lowerLim);
            ball_y_stepper.run();
            break;
        case y_lowerLim:
            ball_y_stepper.moveTo(y_upperLim);
            ball_y_stepper.run();
        default:
            ball_y_stepper.run();
            break;
    }
}

void ball_x_movement(){
    switch (ball_x1_stepper.currentPosition()){
        case x_rightPadEdge:
            ball_x1_stepper.moveTo(x_leftPadEdge);
            ball_x1_stepper.run();
            break;
        case x_leftPadEdge:
            ball_x1_stepper.moveTo(x_rightPadEdge);
            ball_x1_stepper.run();
            break;
        default:
            ball_x1_stepper.run();
            break;
    }
    switch (ball_x2_stepper.currentPosition()){
        case x_rightPadEdge:
            ball_x2_stepper.moveTo(x_leftPadEdge);
            ball_x2_stepper.run();
            break;
        case x_leftPadEdge:
            ball_x2_stepper.moveTo(x_rightPadEdge);
            ball_x2_stepper.run();
            break;
        default:
            ball_x2_stepper.run();
            break;
    }
}

void loop(){
    ball_x_movement();
    ball_y_movement();
    Serial.print(ball_x1_stepper.currentPosition());
    Serial.print("\t");
    Serial.print(ball_x2_stepper.currentPosition());
    Serial.print("\t");
    Serial.println(ball_y_stepper.currentPosition());
}