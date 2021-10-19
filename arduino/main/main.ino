#include <mbed.h>
#include <rtos.h>
#include <SimpleTimer.h>

#include "emg.h"
#include "imu.h"
#include "ble_master.h"


/*** Definition ***/
// #define __COLLECT_RAW_DATA__    true

#define SAMPLES                 256 //Must be a power of 2
#define SAMPLING_FREQUENCY      200 //Hz, must be less than 10000 due to ADC

#define SENSOR_FLAG_A           (1UL << 0)
#define SENSOR_FLAG_B           (1UL << 1)

#define BLE_FLAG_IDLE           (1UL << 0)
#define BLE_FLAG_TOUCH          (1UL << 1)
#define BLE_FLAG_WIPE           (1UL << 2)

#define DBG_BUFFER_SIZE         256
#define MAX_BEACON_NUM          8

#define PORT_BUZZER             9

/*** Name Space ***/
using namespace rtos;

/*** Structure ***/
typedef enum {
  ACTION_NONE   = 0,
  ACTION_TOUCH,
  ACTION_WIPE,
  ACTION_MAX
} ACTION;

typedef struct __attribute__((packed)) {
  uint8_t       addr[6];
  int8_t        rssi;
} BEACON_INFO;

typedef struct __attribute__((packed)) {
  uint8_t       addr[6];
  uint8_t       number_of_beacon;
  BEACON_INFO   beacon[MAX_BEACON_NUM];
  uint8_t       action;
} TX_DATA;


/*** Variables ***/
int           sensor_counter = 0;
int           emg_data[SAMPLES];
SENSORS_DATA  imu_data[SAMPLES];


int           grp_counter = 0;
int           buf_counter = 0;
SimpleTimer   timer_sensor;
Thread        inference_a_thread, inference_b_thread, ble_touch_thread, ble_wipe_thread;
EventFlags    inf_flags, ble_flags;

char          dbg_buf[DBG_BUFFER_SIZE];
char          dbg_max = 0;


/*** Functions ***/
static void ble_touch_thread_cb(){
  TX_DATA       ble_tx_data;

  while(true) {
    ble_flags.wait_all(BLE_FLAG_TOUCH);
  
    memset(&ble_tx_data, 0x00, sizeof(ble_tx_data));
    Serial.print("BLE TX Data Size(TOUCH):");
    Serial.println(sizeof(ble_tx_data));
  
    ble_get_address((char *)ble_tx_data.addr);   
    ble_tx_data.action = (char)ACTION_TOUCH;
    ble_send((byte *)&ble_tx_data, sizeof(ble_tx_data));

    ble_flags.set(BLE_FLAG_IDLE);  
  }
}

static void ble_wipe_thread_cb(){
  TX_DATA       ble_tx_data;

  while(true) {
    ble_flags.wait_all(BLE_FLAG_WIPE);
  
    memset(&ble_tx_data, 0x00, sizeof(ble_tx_data));
    Serial.print("BLE TX Data Size(WIPE) :");
    Serial.println(sizeof(ble_tx_data));
  
    ble_get_address((char *)ble_tx_data.addr);   
    ble_tx_data.action = (char)ACTION_WIPE;
    ble_send((byte *)&ble_tx_data, sizeof(ble_tx_data));

    ble_flags.set(BLE_FLAG_IDLE);
  }
}

static void inference_a_thread_cb(){  
  while(true) {
    ACTION action = ACTION_NONE;
    
    inf_flags.wait_all(SENSOR_FLAG_A, osWaitForever, false);

    // Inference
    // TBD

    // BLE Transmit
    ble_flags.clear(BLE_FLAG_IDLE); // stop sensor
    ble_flags.set(BLE_FLAG_TOUCH);  // start ble thread

    // clear flag
    inf_flags.clear(SENSOR_FLAG_A);
  }
}

static void inference_b_thread_cb(){  
  while(true) {
    ACTION action = ACTION_NONE;

    inf_flags.wait_all(SENSOR_FLAG_B, osWaitForever, false);

    // Inference
    // TBD

    // BLE Transmit
    ble_flags.clear(BLE_FLAG_IDLE); // stop sensor
    ble_flags.set(BLE_FLAG_WIPE);  // start ble thread

    // clear flag
    inf_flags.clear(SENSOR_FLAG_B);
  }
}

static void sensor_task() {
  ble_flags.wait_all(BLE_FLAG_IDLE, osWaitForever, false);
  
  // EMG Sampling(200HZ)
  emg_data[sensor_counter] = emg_read(A0);

  // IMU Sampling(100HZ)
  if((sensor_counter % 2) == 0){
    imu_read(&imu_data[sensor_counter % 2]);
  }

#ifdef __COLLECT_RAW_DATA__
  if((grp_counter % 4) == 0) {
      digitalWrite(PORT_BUZZER, LOW);
  } else if((grp_counter % 4) == 3) || (buf_counter <= SAMPLES/4)) {
      digitalWrite(PORT_BUZZER, HIGH);
  } else {
      digitalWrite(PORT_BUZZER, LOW);
  }

  if((grp_counter % 4) == 0) {
    // print sensor values
    sprintf(dbg_buf, "[DATA]grp:%d, cnt:%d, sample:%d, emg:%f, acc_x:%f, acc_y:%f, acc_z:%f, gyro_x:%f, gyro_y:%f, gyro_z:%f", \
                  grp_counter, buf_counter, SAMPLES, emg, sensors.sensor[SENSOR_KIND_ACC].x, sensors.sensor[SENSOR_KIND_ACC].y, sensors.sensor[SENSOR_KIND_ACC].z, \
                  sensors.sensor[SENSOR_KIND_GYRO].x, sensors.sensor[SENSOR_KIND_GYRO].y, sensors.sensor[SENSOR_KIND_GYRO].z);
    Serial.println(dbg_buf);
  }
#endif

  sensor_counter++;
  if(sensor_counter == SAMPLES){
    inf_flags.set(SENSOR_FLAG_A);
  } else if(sensor_counter == SAMPLES * 2){
    inf_flags.set(SENSOR_FLAG_B);
    sensor_counter = 0;
  }
}

void setup() {
  unsigned int  sampling_period_us = 0;

  digitalWrite(LED_PWR, LOW);

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

  // create ble thread
  ble_touch_thread.start(ble_touch_thread_cb);
  ble_wipe_thread.start(ble_wipe_thread_cb);

  // create inference thread
  inference_a_thread.start(inference_a_thread_cb);
  inference_b_thread.start(inference_b_thread_cb);

  // set ble state
  ble_flags.set(BLE_FLAG_IDLE);

  // set sensor timer interval
  timer_sensor.setInterval(sampling_period_us/1000, sensor_task);
}

void loop() {
  timer_sensor.run();
}
