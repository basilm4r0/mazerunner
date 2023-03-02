#ifndef ENCODERS_H
#define ENCODERS_H

extern const int encb[];
extern volatile long posi[];

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

#endif
