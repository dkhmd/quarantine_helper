#ifndef __emg_h__
#define __emg_h__

#include <ArduinoBLE.h>

void emg_setup();
float emg_read(int port);

#endif
