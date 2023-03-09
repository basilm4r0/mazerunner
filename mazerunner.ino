#include "systick.h"
#include "simplepid.h"
#include "motors.h"
#include "encoders.h"
#include "sensors.h"
#include "config.h"

// Motor Pins
extern const int enca[NMOTORS]; // only pins 2 and 3 are external interrupt capable on arduino uno
extern const int encb[NMOTORS];
extern const int pwm[NMOTORS]; // blue:10
extern const int in1[NMOTORS];
extern const int in2[NMOTORS]; // green:11

// Globals
float prevMillis = 0;
int counter = 0;
long target[NMOTORS] = {0,0};
int cross_track_error = 0;
int error1 = 0;
int error2 = 1;

// PID class instances
extern SimplePID pid[NMOTORS];

int path[PATH_SIZE] = {0};// 1 left, 2 forward, 3 right, 4 backwards
int nodeIdx = 0;// index to last node

// data of distances taken from ultra sonics
float distances_left[5] = {0};
float distances_right[5] = {0};

// medians of distances from left and right
float distance_left;
float distance_right;

void setup() {
  Serial.begin(115200); // prev 115200
  setup_systick();

  for(int k = 0; k < NMOTORS; k++){
    pinMode(enca[k],INPUT);
    pinMode(encb[k],INPUT);
    pinMode(pwm[k],OUTPUT);
    pinMode(in1[k],OUTPUT);
    pinMode(in2[k],OUTPUT);

    pid[k].setParams(kP,kI,kD,255);
  }
    pinMode(irPin1, INPUT);
    pinMode(irPin2, INPUT);
    pinMode(irPinF, INPUT);
    pinMode(trigPin1, OUTPUT);
    pinMode(echoPin1, INPUT);
    pinMode(trigPin2, OUTPUT);
    pinMode(echoPin2, INPUT);

  // TCCR2B = TCCR2B & B11111000 | B00000001; // for PWM frequency of 31372.55 Hz
  bitClear(TCCR1B, CS11);
  bitSet(TCCR1B, CS10);

  attachInterrupt(digitalPinToInterrupt(enca[0]),readEncoder<0>,RISING);
  attachInterrupt(digitalPinToInterrupt(enca[1]),readEncoder<1>,RISING);

  Serial.println("starting");
  // forward.start(MAZE_DIMENSION*PULSES_PER_CM, 8*PULSES_PER_CM, 8*PULSES_PER_CM, 24*PULSES_PER_CM);
  // rotation.start(EIGHTH_TURN_CM*PULSES_PER_CM, 8*PULSES_PER_CM, 0, 4*PULSES_PER_CM);
}

void loop() {
  getUltrasDistances();
  if (distance_left < 5)
    error1--;
  if (distance_right < 5)
    error1++;
  if (distance_left > 10 && distance_left < 20)
    error2++;
  if (distance_right > 10 && distance_right < 20)
    error2--;
  cross_track_error = error1 + 0.75*error2;
  int irPins[3];
  irPins[0] = digitalRead(irPin1);
  irPins[1] = digitalRead(irPin2);
  irPins[2] = digitalRead(irPinF);

  if (irPins[2] == 0){
      forward.stop();
    }

  float currMillis = millis() / 1000;
  if (currMillis - prevMillis >= 1.3) {
    for (int i = 0; i < 10; i++){
    }
      if (irPins[0] == 1 && path[nodeIdx - 1] != 1 && path[nodeIdx - 1] != 3 && path[nodeIdx - 1] != 4) {
          turn_left(currMillis);
          path[nodeIdx] = 1;
          update_idx();
      } else if (irPins[2] == 1) {
          move_forward(currMillis);
          path[nodeIdx] = 2;
          update_idx();
      } else if (irPins[1] == 1) {
          turn_right(currMillis);
          path[nodeIdx] = 3;
          update_idx();
      } else {
          turn_around(currMillis);
          path[nodeIdx] = 4;
          update_idx();
      }
  }
  // for (int k = 0; k < NMOTORS; k++) {
  //   Serial.print("pos ");
  //   Serial.print(k);
  //   Serial.print(":");
  //   Serial.print(pos[k]);
  //   Serial.print(",");
  // }
  // Serial.println();
}

void move_forward(float currMillis) {
    forward.start(MAZE_DIMENSION * PULSES_PER_CM, 30 * PULSES_PER_CM, 0, 96 * PULSES_PER_CM);
    prevMillis = currMillis;
    // Serial.println("going forward");
}

void turn_left(float currMillis) {
    rotation.start(EIGHTH_TURN_CM * PULSES_PER_CM, 30 * PULSES_PER_CM, 0, 96 * PULSES_PER_CM);
    prevMillis = currMillis;
    // Serial.println("turning left");
}

void turn_right(float currMillis) {
    rotation.start(-EIGHTH_TURN_CM * PULSES_PER_CM, 30 * PULSES_PER_CM, 0, 96 * PULSES_PER_CM);
    prevMillis = currMillis;
    // Serial.println("turning right");
}

void turn_around(float currMillis) {
    rotation.start(2 * EIGHTH_TURN_CM * PULSES_PER_CM, 30 * PULSES_PER_CM, 0, 96 * PULSES_PER_CM);
    prevMillis = currMillis;
    // Serial.println("going backwards :(");
}

void is_it_finished(Profile motion) {
  while (motion.is_finished() == false) {

  }
}

void update_idx() {
    nodeIdx = (nodeIdx + 1) % PATH_SIZE;
}

// Calculate the median value of an array
float median(float arr[], int size) {
    if (size % 2 == 0) {
        // Average of the two middle values if there are an even number of elements
        return (arr[size / 2 - 1] + arr[size / 2]) / 2.0;
    } else {
        // Middle value if there are an odd number of elements
        return arr[size / 2];
    }
}

void getUltrasDistances() {
    float distances_left[10] = {0};
    float distances_right[10] = {0};

    // Take 10 measurements for each ultrasonic sensor
    for (int i = 0; i < 10; i++) {
        distances_left[i] = measureDistance(echoPin1, trigPin1);
        distances_right[i] = measureDistance(echoPin2, trigPin2);
    }

    // Bubble sort the arrays of distances
    int n = 3;
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (distances_left[j] > distances_left[j + 1]) {
                float temp = distances_left[j];
                distances_left[j] = distances_left[j + 1];
                distances_left[j + 1] = temp;
            }
            if (distances_right[j] > distances_right[j + 1]) {
                float temp = distances_right[j];
                distances_right[j] = distances_right[j + 1];
                distances_right[j + 1] = temp;
            }
        }
    }

    // Calculate the median distance for both left and right
    distance_left = median(distances_left, 10);
    distance_right = median(distances_right, 10);
}
