
#ifndef CONFIG_H
#define CONFIG_H

// Constants
#define MAZE_DIMENSION 25
#define NMOTORS 2
#define PULSES_PER_CM 16
#define EIGHTH_TURN_CM 7.1
#define WHEEL_PERIMETER_ERROR -0.1
#define kP 5
#define kI 0.1
#define kD 0.1

// Sensor pins
#define irPin1 8
#define irPin2 5
#define trigPin1 A5
#define echoPin1 A4
#define trigPin2 A3
#define echoPin2 A2
#define trigPin3 A1
#define echoPin3 A0

// Motor Pins
const int enca[NMOTORS] = {2,3}; // only pins 2 and 3 are external interrupt capable on arduino uno
const int encb[NMOTORS] = {12,13};
const int pwm[NMOTORS] = {10,9}; // blue:10
const int in1[NMOTORS] = {6,7};
const int in2[NMOTORS] = {11,4}; // green:11

// Control loop timing. Pre-calculate to save time in interrupts
const float LOOP_FREQUENCY = 500.0;
const float LOOP_INTERVAL = (1.0 / LOOP_FREQUENCY);

#endif
