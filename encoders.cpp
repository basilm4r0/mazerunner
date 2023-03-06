#include "encoders.h"
#include "config.h"
#include "Arduino.h"
#include <util/atomic.h>

extern const int encb[];
volatile long posi[NMOTORS] = {0,0};
long pos[NMOTORS];

// Read the position in an atomic block to avoid a potential misread
void update_position() {
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE){
    for(int k = 0; k < NMOTORS; k++){
      pos[k] = posi[k];
    }
  }
}
