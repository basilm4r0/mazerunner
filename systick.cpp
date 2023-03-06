#include "systick.h"
#include "encoders.h"
#include "motors.h"
#include "profile.h"
#include "config.h"
// #include "sensors.h"
#include <Arduino.h>

void setup_systick() {
  bitClear(TCCR2A, WGM20);
  bitSet(TCCR2A, WGM21);
  bitClear(TCCR2B, WGM22);
  // set divisor to 128 => 125kHz
  bitSet(TCCR2B, CS22);
  bitClear(TCCR2B, CS21);
  bitSet(TCCR2B, CS20);
  OCR2A = 249; // (16000000/128/500)-1 => 500Hz
  bitSet(TIMSK2, OCIE2A);
}

/***
 * This is the SYSTICK ISR. It runs at 500Hz by default.
 *
 * All the time-critical control functions happen in here.
 *
 * interrupts are enabled at the start of the ISR so that encoder
 * counts are not lost.
 *
 * The last thing it does is to start the sensor reads so that they
 * will be ready to use next time around.
 *
 * Timing tests indicate that, with the robot at rest, the systick ISR
 * consumes about 10% of the available system bandwidth.
 *
 * With just a single profile active and moving, that increases to nearly 30%.
 * Two such active profiles increases it to about 35-40%.
 *
 * The reason that two profiles does not take up twice as much time is that
 * an active profile has a processing overhead even if there is no motion.
 *
 * Most of the load is due to that overhead. While the profile generates actual
 * motion, there is an additional load.
 *
 *
 */
ISR(TIMER2_COMPA_vect, ISR_NOBLOCK) {
  // TODO: make sure all variables are interrupt-safe if they are used outside IRQs
  // grab the encoder values first because they will continue to change
  update_position();
  forward.update();
  rotation.update();
  update_motors();
  // g_cross_track_error = update_wall_sensors();
  // g_steering_adjustment = calculate_steering_adjustment(g_cross_track_error);
  // update_motor_controllers(g_steering_adjustment);
  // start_sensor_cycle();
  // NOTE: no code should follow this line;
}
