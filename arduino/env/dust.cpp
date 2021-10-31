#include "dust.h"

unsigned long t0;

static double pcs2ugm3 (double pcs)
{
  double pi = 3.14159;
  double density = 1.65 * pow (10, 12);
  double r25 = 2.5 * pow (10, -6);
  double vol25 = (4/3) * pi * pow (r25, 3);
  double mass25 = density * vol25; // μg
  double K = 3531.5; // per m^3 

  return pcs * K * mass25;
}

void dust_setup(int pin) {
  pinMode(pin, INPUT);
}

bool dust_read(int pin, unsigned long ts, double *dust) {
  double concent = 0;
  unsigned long lowOc = 0;
  double ratio = 0;

  // LOW occupancy
  lowOc = pulseIn(pin, LOW);

  // LOW Ratio
  ratio = lowOc / (ts * 10.0);
  // Calculate Content
  concent = 1.1 * pow(ratio, 3) - 3.8 * pow(ratio, 2) + 520 * ratio + 0.62;

  *dust = pcs2ugm3(concent);

  return true;
}
