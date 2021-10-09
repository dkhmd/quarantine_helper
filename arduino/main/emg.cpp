#include "emg.h"

unsigned int sampling_period_us;
double emg_data[SAMPLES];

void setup_emg() {
  sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}

double *loop_emg(int port) { 
  unsigned long microseconds = 0;
 
  /*SAMPLING*/
  for(int i=0; i<SAMPLES; i++)
  {
    microseconds = micros();    //Overflows after around 70 minutes!
 
    emg_data[i] = analogRead(port);
//    while(micros() < (microseconds + sampling_period_us)){
//      yield();
//    }
  }

  return emg_data;
}
