#include "emg.h"

void setup_emg() {
}

float read_emg(int port) { 
  int sensorValue;
  float voltage;
  
  sensorValue = analogRead(port);
  voltage = sensorValue * (5.0 / 1023.0);

  return voltage;
}
