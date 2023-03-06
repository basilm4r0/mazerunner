#include "config.h"
#include "simplepid.h"
#include "encoders.h"
#include "profile.h"
#include "Arduino.h"
#ifndef MOTORS_H
#define MOTORS_H

void setMotor(int dir, int pwmVal, int pwm, int in1, int in2);

void update_motors();

#endif
