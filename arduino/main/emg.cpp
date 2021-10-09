#include "emg.h"

void setup_emg() {
}

double read_emg(int port) { 
  double emg_data;
  
  emg_data = analogRead(port);

  return emg_data;
}
