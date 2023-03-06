#include "simplepid.h"
#include "config.h"
#include <Arduino.h>

// PID class instances
SimplePID pid[NMOTORS];

long prevT = 0;

// time difference
float time_difference() {
  long currT = micros();
  float deltaT = ((float) (currT - prevT))/( 1.0e6 );
  prevT = currT;
  return deltaT;
}
