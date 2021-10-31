#include "dust.h"

void dust_setup(int pin) {
  pinMode(pin, INPUT);
}

float dust_read(int pin, unsigned long sampletime_ms) { 
  unsigned long duration = 0;
  unsigned long lowpulseoccupancy = 0;
  float ratio = 0;

  float concentration = 0;

  duration = pulseIn(pin, LOW);
  lowpulseoccupancy = lowpulseoccupancy + duration;

  ratio = lowpulseoccupancy / (sampletime_ms * 10.0);
  concentration = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;

  return concentration;
}
