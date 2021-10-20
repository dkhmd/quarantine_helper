#include "emg.h"

void emg_setup() {
}

float emg_read(int port) { 
  int sensorValue = 0;
  float voltage = 0;
  
  sensorValue = analogRead(port);
  voltage = ((float)sensorValue) * 5 / 1023;

  return voltage;
}
