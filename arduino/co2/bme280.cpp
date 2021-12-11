#include "bme280.h"

BME280 sensor;

void bme280_setup() {
  sensor.beginI2C();
}
 
bool bm280_read(float *temp, float *hum, float *pressure) {
  *temp = sensor.readTempC();
  *hum = sensor.readFloatHumidity();
  *pressure = sensor.readFloatPressure() / 100.0;
  
  return true;
}
