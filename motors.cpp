#include "motors.h"
#include "config.h"
#include "simplepid.h"
#include "encoders.h"
#include "profile.h"
#include "Arduino.h"

Profile forward;
Profile rotation;

extern volatile long posi[NMOTORS];
extern long target[NMOTORS];
extern long pos[NMOTORS];
extern SimplePID pid[NMOTORS];

// Motor Pins
extern const int enca[NMOTORS]; // only pins 2 and 3 are external interrupt capable on arduino uno
extern const int encb[NMOTORS];
extern const int pwm[NMOTORS];  // blue:10
extern const int in1[NMOTORS];
extern const int in2[NMOTORS];  // green:11

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2){
  analogWrite(pwm,pwmVal);
  if(dir == 1){
    digitalWrite(in1,HIGH);
    digitalWrite(in2,LOW);
  }
  else if(dir == -1){
    digitalWrite(in1,LOW);
    digitalWrite(in2,HIGH);
  }
  else{
    digitalWrite(in1,LOW);
    digitalWrite(in2,LOW);
  }
}

// loop through the motors
void update_motors() {
  int pwr, dir, target;
  float deltaT = time_difference();
  for (int k = 0; k < NMOTORS; k++) {
    target = (forward.position()+(2*k-1)*rotation.position());
    // evaluate the control signal
    pid[k].evalu(pos[k], (1+k * WHEEL_PERIMETER_ERROR) * target, deltaT, pwr, dir);
    // signal the motor
    setMotor(dir,pwr,pwm[k],in1[k],in2[k]);
    Serial.print("pos ");
    Serial.print(k);
    Serial.print(":");
    Serial.print(target);
    Serial.print(",");
  }
  Serial.println();

}
