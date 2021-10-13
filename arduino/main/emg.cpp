#include "emg.h"

void emg_setup() {
}

int emg_read(int port) { 
  int sensorValue;
  
  sensorValue = analogRead(port);

  return sensorValue;
}
