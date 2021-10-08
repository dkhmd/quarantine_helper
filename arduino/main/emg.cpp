#include "emg.h"

#define SAMPLES 1024            //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;
 
void setup_emg() {
  sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
}
 
void loop_emg(int port) { 
  unsigned long microseconds = 0;
  double val = 0;
 
  /*SAMPLING*/
  for(int i=0; i<SAMPLES; i++)
  {
      microseconds = micros();    //Overflows after around 70 minutes!
   
      val = analogRead(port);

//      Serial.println(val);
      while(micros() < (microseconds + sampling_period_us)){
        yield();
      }
  }
}
