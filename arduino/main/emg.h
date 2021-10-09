#ifndef __emg_h__
#define __emg_h__

#include <ArduinoBLE.h>

#define SAMPLES 1024            //Must be a power of 2
#define SAMPLING_FREQUENCY 1000 //Hz, must be less than 10000 due to ADC

void setup_emg();
double *loop_emg(int port);

#endif
