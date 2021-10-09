#include "emg.h"
#include "imu.h"
#include "ble_master.h"

#define SAMPLES 256             //Must be a power of 2
#define SAMPLING_FREQUENCY 200  //Hz, must be less than 10000 due to ADC

unsigned int sampling_period_us;

void setup() {
  Serial.begin(115200);
  while (!Serial);

  sampling_period_us = round(1000000*(1.0/SAMPLING_FREQUENCY));
  Serial.print("Sampling Period:");
  Serial.println(sampling_period_us);

  setup_emg();
  setup_imu();

  setup_ble_master();
}
 
void loop() {
  char        buf[256];
  
  unsigned long microseconds = 0;
  float         emg = 0;
  SENSORS_DATA  sensors;
  
  // Sampling
  for(int i=0; i<SAMPLES; i++)
  {
    microseconds = micros();    //Overflows after around 70 minutes!

    emg = read_emg(A0);
    read_imu(&sensors);

    sprintf(buf, "i: %d, emg: %f acc_x:%f, acc_y:%f, acc_z:%f, gyro_x:%f, gyro_y:%f, gyro_z:%f", \
                  i, emg, sensors.sensor[SENSOR_KIND_ACC].x, sensors.sensor[SENSOR_KIND_ACC].y, sensors.sensor[SENSOR_KIND_ACC].z, \
                  sensors.sensor[SENSOR_KIND_GYRO].x, sensors.sensor[SENSOR_KIND_GYRO].y, sensors.sensor[SENSOR_KIND_GYRO].z);
    Serial.println(buf);

    while(micros() < (microseconds + sampling_period_us)){
      yield();
    }
  }

  // Inference

  // Transmit
  loop_ble_master();
}
