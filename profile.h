#ifndef PROFILE_H
#define PROFILE_H

#include "encoders.h"
#include "config.h"
#include <Arduino.h>
#include <util/atomic.h>
//***************************************************************************//
class Profile;

extern Profile forward;
extern Profile rotation;

enum ProfileState : uint8_t {
  CS_IDLE = 0,
  CS_ACCELERATING = 1,
  CS_BRAKING = 2,
  CS_FINISHED = 3,
};

class Profile {
  public:
  void reset() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      m_position = 0;
      m_speed = 0;
      m_target_speed = 0;
      m_state = CS_IDLE;
    }
  }

  void clear_counters() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      m_position = 0;
    }
  }

  bool is_finished() { return m_state == CS_FINISHED; }

  void start(float distance, float top_speed, float final_speed, float acceleration) {
    m_sign = (distance < 0) ? -1 : +1;
    if (distance < 0) {
      distance = -distance;
    }
    if (distance < 1.0) {
      m_state = CS_FINISHED;
      return;
    }
    if (final_speed > top_speed) {
      final_speed = top_speed;
    }

    // m_position = 0;
    m_final_position = m_position + m_sign * distance;
    m_target_speed = m_sign * fabsf(top_speed);
    m_final_speed = m_sign * fabsf(final_speed);
    m_acceleration = fabsf(acceleration);
    if (m_acceleration >= 1) {
      m_one_over_acc = 1.0f / m_acceleration;
    } else {
      m_one_over_acc = 1.0;
    }
    m_state = CS_ACCELERATING;
  }

  void stop() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      m_target_speed = 0;
    }
    finish();
  }

  void finish() {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      m_speed = m_target_speed;
      m_state = CS_FINISHED;
    }
  }

  void set_state(ProfileState state) { m_state = state; }

  float get_braking_distance() {
    return fabsf(m_speed * m_speed - m_final_speed * m_final_speed) * 0.5 * m_one_over_acc;
  }

  float position() {
    float pos;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      pos = m_position;
    }
    return pos;
  }

  float speed() {
    float speed;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      speed = m_speed;
    }
    return speed;
  }

  float increment() {
    float inc;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      inc = m_speed * LOOP_INTERVAL;
    }
    return inc;
  }

  float acceleration() {
    float acc;
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      acc = m_acceleration;
    }
    return acc;
  }

  void set_speed(float speed) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      m_speed = speed;
    }
  }
  void set_target_speed(float speed) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
      m_target_speed = speed;
    }
  }

  // normally only used to alter position for forward error correction
  void adjust_position(float adjustment) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { m_position += adjustment; }
  }

  void set_position(float position) {
    ATOMIC_BLOCK(ATOMIC_RESTORESTATE) { m_position = position; }
  }

  // update is called from within systick and should be safe from interrupts
  void update() {
    if (m_state == CS_IDLE) {
      return;
    }
    float delta_v = m_acceleration * LOOP_INTERVAL;
    float remaining = fabsf(m_final_position - m_position);
    if (m_state == CS_ACCELERATING) {
      if (remaining < get_braking_distance()) {
        m_state = CS_BRAKING;
        if (m_final_speed == 0) {
          m_target_speed = m_sign * 5.0f;
        } else {
          m_target_speed = m_final_speed;
        };
      }
    }
    // try to reach the target speed
    if (m_speed < m_target_speed) {
      m_speed += delta_v;
      if (m_speed > m_target_speed) {
        m_speed = m_target_speed;
      }
    }
    if (m_speed > m_target_speed) {
      m_speed -= delta_v;
      if (m_speed < m_target_speed) {
        m_speed = m_target_speed;
      }
    }
    // increment the position
    m_position += m_speed * LOOP_INTERVAL;
    if (m_state != CS_FINISHED && remaining < 0.125) {
      m_state = CS_FINISHED;
      m_target_speed = m_final_speed;
    }
  }

  private:
  volatile uint8_t m_state = CS_IDLE;
  volatile float m_speed = 0;
  volatile float m_position = 0;
  int8_t m_sign = 1;
  float m_acceleration = 0;
  float m_one_over_acc = 1;
  float m_target_speed = 0;
  float m_final_speed = 0;
  float m_final_position = 0;
};

#endif
