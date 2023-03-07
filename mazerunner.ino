#include "systick.h"
#include "simplepid.h"
#include "motors.h"
#include "encoders.h"
#include "config.h"

// Motor Pins
extern const int enca[NMOTORS]; // only pins 2 and 3 are external interrupt capable on arduino uno
extern const int encb[NMOTORS];
extern const int pwm[NMOTORS]; // blue:10
extern const int in1[NMOTORS];
extern const int in2[NMOTORS]; // green:11

// Globals
long prevMillis = 0;
int counter = 0;
long target[NMOTORS] = {0,0};

// PID class instances
extern SimplePID pid[NMOTORS];

void setup() {
  Serial.begin(115200);
  setup_systick();

  for(int k = 0; k < NMOTORS; k++){
    pinMode(enca[k],INPUT);
    pinMode(encb[k],INPUT);
    pinMode(pwm[k],OUTPUT);
    pinMode(in1[k],OUTPUT);
    pinMode(in2[k],OUTPUT);

    pid[k].setParams(kp,ki,kd,255); // 1,0.3,-0.01
  }

  // TCCR2B = TCCR2B & B11111000 | B00000001; // for PWM frequency of 31372.55 Hz
  bitClear(TCCR1B, CS11);
  bitSet(TCCR1B, CS10);

  attachInterrupt(digitalPinToInterrupt(enca[0]),readEncoder<0>,RISING);
  attachInterrupt(digitalPinToInterrupt(enca[1]),readEncoder<1>,RISING);

  Serial.println("starting forward profile");
  // forward.start(MAZE_DIMENSION*PULSES_PER_CM, 8*PULSES_PER_CM, 8*PULSES_PER_CM, 24*PULSES_PER_CM);
  // rotation.start(EIGHTH_TURN_CM*PULSES_PER_CM, 8*PULSES_PER_CM, 0, 4*PULSES_PER_CM);
}

void loop() {


  long currMillis = millis();
  if (currMillis - prevMillis >= 3000) {
    if (counter == 0) {
      // rotation.start(-EIGHTH_TURN_CM*PULSES_PER_CM, 8*PULSES_PER_CM, 0, 4*PULSES_PER_CM);
      forward.start(2*MAZE_DIMENSION*PULSES_PER_CM, 24*PULSES_PER_CM, 0, 24*PULSES_PER_CM);
      Serial.println("turn");
      prevMillis = currMillis;
      counter = 1;
    } else if (counter == 1) {
      rotation.start(2*EIGHTH_TURN_CM*PULSES_PER_CM, 24*PULSES_PER_CM, 0, 24*PULSES_PER_CM);
      // forward.start(-MAZE_DIMENSION*PULSES_PER_CM, 24*PULSES_PER_CM, 0, 24*PULSES_PER_CM);
      prevMillis = currMillis;
      counter--;
      Serial.println("turn");
    }
  }
}

float measureDistance(int echoPin, int trigPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  long duration = pulseIn(echoPin, HIGH);
  float distance = duration * 0.0344 / 2;
  return distance;
}
