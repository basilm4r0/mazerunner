#include <util/atomic.h>
#include "systick.h"
#include "simplepid.h"
#include "motors.h"
#include "encoders.h"

// How many motors
#define MAZE_DIMENSION 18.2
#define NMOTORS 2
#define PULSES_PER_CM 14.43
#define EIGHTH_TURN_CM 8

// Pins
const int enca[] = {2,3}; // only pins 2 and 3 are external interrupt capable on arduino uno
const int encb[] = {12,13};
const int pwm[] = {10,5};
const int in1[] = {6,7};
const int in2[] = {11,4};

// Globals
long prevT = 0;
volatile long posi[] = {0,0};
long prevMillis = 0;
int counter = 0;
int target[NMOTORS] = {0,0};

// PID class instances
SimplePID pid[NMOTORS];

void setup() {
  Serial.begin(9600);
  setup_systick();

  for(int k = 0; k < NMOTORS; k++){
    pinMode(enca[k],INPUT);
    pinMode(encb[k],INPUT);
    pinMode(pwm[k],OUTPUT);
    pinMode(in1[k],OUTPUT);
    pinMode(in2[k],OUTPUT);

    pid[k].setParams(2,0.3,-0.01,255); // 1,0.3,-0.01
  }

  attachInterrupt(digitalPinToInterrupt(enca[0]),readEncoder<0>,RISING);
  attachInterrupt(digitalPinToInterrupt(enca[1]),readEncoder<1>,RISING);
}

void loop() {

  long currMillis = millis();
  if (currMillis - prevMillis >= 1000) {
    if (counter == 0) {
      target[0] += int(MAZE_DIMENSION * PULSES_PER_CM);
      target[1] += int(MAZE_DIMENSION * PULSES_PER_CM);
      Serial.println("go forward");
      prevMillis = currMillis;
      counter++;
    } else if (counter == 1) {
      //target[0] -= int(EIGHTH_TURN_CM * PULSES_PER_CM);
      target[1] += int(2 * EIGHTH_TURN_CM * PULSES_PER_CM);
      prevMillis = currMillis;
      counter--;
      Serial.println("turn");
    }
  }
  // time difference
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;

  // Read the position in an atomic block to avoid a potential misread
  long pos[NMOTORS];
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    for(int k = 0; k < NMOTORS; k++){
      pos[k] = posi[k];
    }
  }

  // loop through the motors
  for(int k = 0; k < NMOTORS; k++){
    int pwr, dir;
    // evaluate the control signal
    pid[k].evalu(pos[k],target[k],deltaT,pwr,dir);
    // signal the motor
    setMotor(dir,pwr,pwm[k],in1[k],in2[k]);
    Serial.print("pwr");
    Serial.print(k);
    Serial.print(":");
    Serial.print(pwr);
    Serial.print(",");
  }

  //Serial plotter graphing
  Serial.print("target0:");
  Serial.print(target[0]);
  Serial.print(",");
  Serial.print("pos0:");
  Serial.print(pos[0]);
  Serial.print(",");
  Serial.print("target1:");
  Serial.print(target[1]);
  Serial.print(",");
  Serial.print("pos1:");
  Serial.print(pos[1]);
  Serial.println();
}
