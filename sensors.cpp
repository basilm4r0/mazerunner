#include "sensors.h"
#include "Arduino.h"

// measure distance with ultrasonic sensor
float measureDistance(int echoPin, int trigPin) {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin,LOW);
  long duration = pulseIn(echoPin, HIGH, 38000);
  float distance = duration * 0.034 / 2;
  if (distance == 0)
    distance = 200;
  return distance;
}
