#include <SimpleTimer.h>

#include "emg.h"
#include "imu.h"
#include "ble_master.h"


//#define __COLLECT_DEBUG_DATA__

#define SAMPLES 256             //Must be a power of 2
#define SAMPLING_FREQUENCY 200  //Hz, must be less than 10000 due to ADC


SimpleTimer   timer;
int           buf_counter;
unsigned int  sampling_period_us;


static void RepeatTask() {
  char          buf[256];
  float         emg = 0;
  SENSORS_DATA  sensors;

  // Sampling
  emg = read_emg(A0);
  read_imu(&sensors);

#ifdef __DEBUG_COLLECT_DATA__
  sprintf(buf, "counter:%d, emg:%f acc_x:%f, acc_y:%f, acc_z:%f, gyro_x:%f, gyro_y:%f, gyro_z:%f", \
                buf_counter, emg, sensors.sensor[SENSOR_KIND_ACC].x, sensors.sensor[SENSOR_KIND_ACC].y, sensors.sensor[SENSOR_KIND_ACC].z, \
                sensors.sensor[SENSOR_KIND_GYRO].x, sensors.sensor[SENSOR_KIND_GYRO].y, sensors.sensor[SENSOR_KIND_GYRO].z);
  Serial.println(buf);
#endif

  //T.B.D
  buf_counter++;
  if(buf_counter >= 256){
    buf_counter = 0;
  }

  // Inference
}

void setup() {
  Serial.begin(115200);
  while (!Serial);

  // set gloval variables
  sampling_period_us = round(1000000 * (1.0/SAMPLING_FREQUENCY));
  Serial.print("Sampling Period:");
  Serial.println(sampling_period_us);

  // setup each device
  setup_emg();
  setup_imu();

  setup_ble_master();

  // set timer interval
  timer.setInterval(sampling_period_us/1000, RepeatTask);
}
 
void loop() {
//  // Transmit
//  loop_ble_master();
  timer.run();
}
