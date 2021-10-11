#include <SimpleTimer.h>

#include "emg.h"
#include "imu.h"
#include "ble_master.h"


/*** Definition ***/
//#define __COLLECT_DEBUG_DATA__

#define SAMPLES             256 //Must be a power of 2
#define SAMPLING_FREQUENCY  200 //Hz, must be less than 10000 due to ADC
#define DBG_BUFFER_SIZE     128
#define MAX_BEACON_NUM      8


/*** Structure ***/
typedef struct __attribute__((packed)) {
  uint8_t mac[6];
  int8_t  rssi;
}BEACON_INFO;

typedef struct __attribute__((packed)) {
  uint8_t       mac[6];
  uint8_t       number_of_beacon;
  BEACON_INFO   beacon[MAX_BEACON_NUM];
  uint8_t action;
}TX_DATA;


/*** Variables ***/
SimpleTimer   timer;
char          dbg_buf[DBG_BUFFER_SIZE];
int           buf_counter;
TX_DATA       ble_tx_data;


static void repeat_task() {
  float         emg = 0;
  SENSORS_DATA  sensors;
  unsigned long micros_start = 0;

  // change thread priority
  osThreadSetPriority(osThreadGetId(), osPriorityRealtime);

  // EMG Sampling
//  micros_start = micros();
  emg = emg_read(A0);
//  Serial.print("EMG:");
//  Serial.print(micros() - micros_start);
//  Serial.print(",");

  // IMU Sampling
//  micros_start = micros();
  imu_read(&sensors);
//  Serial.print("IMU:");
//  Serial.print(micros() - micros_start);

  sprintf(dbg_buf, "cnt:%d, emg:%f, acc_x:%f, acc_y:%f, acc_z:%f, gyro_x:%f, gyro_y:%f, gyro_z:%f", \
                buf_counter, emg, sensors.sensor[SENSOR_KIND_ACC].x, sensors.sensor[SENSOR_KIND_ACC].y, sensors.sensor[SENSOR_KIND_ACC].z, \
                sensors.sensor[SENSOR_KIND_GYRO].x, sensors.sensor[SENSOR_KIND_GYRO].y, sensors.sensor[SENSOR_KIND_GYRO].z);
  Serial.println(dbg_buf);

//  Serial.println("");

  buf_counter++;

  //T.B.D
  if(buf_counter >= SAMPLES){
    buf_counter = 0;

#if 0
    // Inference
    
    // Transmit
    memset(&ble_tx_data, 0x00, sizeof(ble_tx_data));
    Serial.print("Data Size:");
    Serial.print(sizeof(ble_tx_data));
    Serial.print(",");

    micros_start = micros();
//    ble_send((byte *)&ble_tx_data, sizeof(ble_tx_data));
    ble_send((byte *)dbg_buf, sizeof(dbg_buf));
    Serial.print("BLE:");
    Serial.print(micros() - micros_start);

    Serial.println("");
#endif
  }
}

void setup() {
  unsigned int  sampling_period_us;

  // initialize serial
  Serial.begin(115200);
  while (!Serial);

  // set gloval variables
  sampling_period_us = round(1000000 * (1.0/SAMPLING_FREQUENCY));
  Serial.print("Sampling Period:");
  Serial.println(sampling_period_us);

  // setup each device
  emg_setup();
  imu_setup();

  ble_setup();

  // set timer interval
  timer.setInterval(sampling_period_us/1000, repeat_task);
}
 
void loop() {
  timer.run();
}
