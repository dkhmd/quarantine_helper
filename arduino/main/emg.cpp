#include "emg.h"

void emg_setup() {
}

float emg_read(int port) { 
  int sensorValue;
  float voltage;
  
  sensorValue = analogRead(port);
  voltage = sensorValue * (5.0 / 1023.0);

  return voltage;
}
