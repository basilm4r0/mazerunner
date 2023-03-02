#include <util/atomic.h>

// A class to compute the control signal
class SimplePID{
  private:
    float kp, kd, ki, umax; // Parameters
    float eprev, eintegral; // Storage

  public:
  // Constructor
  SimplePID() : kp(1), kd(0), ki(0), umax(255), eprev(0.0), eintegral(0.0){}

  // A function to set the parameters
  void setParams(float kpIn, float kdIn, float kiIn, float umaxIn){
    kp = kpIn; kd = kdIn; ki = kiIn; umax = umaxIn;
  }

  // A function to compute the control signal
  void evalu(int value, int target, float deltaT, int &pwr, int &dir){
    // error
    int e = target - value;

    // derivative
    float dedt = (e-eprev)/(deltaT);

    // integral
    eintegral = eintegral + e*deltaT;

    // control signal
    float u = kp*e + kd*dedt + ki*eintegral;

    // motor power
    pwr = (int) fabs(u);
    if( pwr > umax ){
      pwr = umax;
    }

    // motor direction
    dir = 1;
    if(u<0){
      dir = -1;
    }

    // store previous error
    eprev = e;
  }

};

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

template <int j>
void readEncoder(){
  int b = digitalRead(encb[j]);
  if(b > 0){
    posi[j]++;
  }
  else{
    posi[j]--;
  }
}
