#include <Arduino.h>

#define STEP_PIN 6
#define DIR_PIN 7


void setup() {
  pinMode(DIR_PIN,    OUTPUT);
  pinMode(STEP_PIN,   OUTPUT);
}

void simpleAccel(int steps) {

  int lowSpeed = 3000;
  int highSpeed = 100;
  int change = 1;

  int rampUpStop = (lowSpeed - highSpeed) / change;
  if ( rampUpStop > steps / 2 )
    rampUpStop = steps / 2;
  int rampDownStart = steps - rampUpStop;

  int d = lowSpeed;

  for (int i = 0; i < steps; i++) {
    digitalWrite(STEP_PIN, HIGH);
    delay(1);
    digitalWrite(STEP_PIN, LOW);
    delayMicroseconds(d);

    if ( i < rampUpStop )
      d -= change;
    else if ( i > rampDownStart )
      d += change;
  }
}

void loop() {
  digitalWrite(DIR_PIN, LOW);
  simpleAccel(4800);
  digitalWrite(DIR_PIN, HIGH);
  simpleAccel(4800);

  while (true);

}
