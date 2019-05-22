#include <Arduino.h>

#define STEP_PIN 6
#define DIR_PIN 7


void setup() {
  pinMode(DIR_PIN,    OUTPUT);
  pinMode(STEP_PIN,   OUTPUT);
}

void loop() {
  digitalWrite(STEP_PIN, HIGH);
  delay(1);
  digitalWrite(STEP_PIN, LOW);
  delay(1);
}

//12V speed= 70rpm
//24V speed= 70rpm