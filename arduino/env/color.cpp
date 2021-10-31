#include "color.h"

void color_setup() {
  if (!APDS.begin()) {
    Serial.println("Error initializing APDS-9960 sensor.");
  }
}

bool color_read(int* r, int* g, int* b) {
  while (! APDS.colorAvailable()) {
    return false;
  }

  APDS.readColor(*r, *g, *b);

  return true;
}
