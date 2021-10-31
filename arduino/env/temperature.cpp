#include "temperature.h"

void temperature_setup() {
  if (!HTS.begin()) {
    Serial.println("Failed to initialize humidity temperature sensor!");
    while (1);
  }
}

bool temperature_read(float* temperature, float* humidity) { 
  *temperature = HTS.readTemperature();
  *humidity  = HTS.readHumidity();

  return true;
}
