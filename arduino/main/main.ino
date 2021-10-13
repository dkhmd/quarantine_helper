#include <mbed.h>
#include <rtos.h>
#include <SimpleTimer.h>

#include "emg.h"
#include "imu.h"
#include "ble_master.h"


/*** Definition ***/
// #define __COLLECT_RAW_DATA__    true

#define EMG_SAMPLES             256 //Must be a power of 2
#define EMG_SAMPLING_FREQUENCY  200 //Hz, must be less than 10000 due to ADC
#define IMU_SAMPLES             128
#define IMU_SAMPLING_FREQUENCY  100

#define ACTION_TOUCH_FLAG       (1UL << 0)
#define ACTION_WIPE_FLAG        (1UL << 1)

#define DBG_BUFFER_SIZE         256
#define MAX_BEACON_NUM          8

#define PORT_BUZZER             9

/*** Name Space ***/
using namespace rtos;

/*** Structure ***/
typedef enum {
  ACTION_NON    = 0,
  ACTION_TOUCH,
  ACTION_WIPE,
  ACTION_MAX
} ACTION;

typedef struct __attribute__((packed)) {
  uint8_t addr[6];
  int8_t  rssi;
} BEACON_INFO;

typedef struct __attribute__((packed)) {
  uint8_t       addr[6];
  uint8_t       number_of_beacon;
  BEACON_INFO   beacon[MAX_BEACON_NUM];
  uint8_t action;
} TX_DATA;


/*** Variables ***/
int           grp_counter = 0;
int           buf_counter = 0;
SimpleTimer   timer_emg, timer_imu;
Thread        thread_touch, thread_wipe;
EventFlags    event_flags;

char          dbg_buf[DBG_BUFFER_SIZE];
char          dbg_max = 0;


void ble_thread_touch() {
    TX_DATA       ble_tx_data;

//    osThreadSetPriority(osThreadGetId(), osPriorityHigh);

    while(true) {
      event_flags.wait_any(ACTION_TOUCH_FLAG);
      
      // Transmit
      memset(&ble_tx_data, 0x00, sizeof(ble_tx_data));
      Serial.print("BLE TX Data Size:");
      Serial.println(sizeof(ble_tx_data));

      ble_get_address((char *)ble_tx_data.addr);   
      ble_tx_data.action = (char)ACTION_TOUCH;
      ble_send((byte *)&ble_tx_data, sizeof(ble_tx_data));
    }
}

void ble_thread_wipe() {
    TX_DATA       ble_tx_data;

    while(true) {
      event_flags.wait_any(ACTION_WIPE_FLAG);
      
      // Transmit
      memset(&ble_tx_data, 0x00, sizeof(ble_tx_data));
      Serial.print("BLE TX Data Size:");
      Serial.println(sizeof(ble_tx_data));

      ble_get_address((char *)ble_tx_data.addr);  
      ble_tx_data.action = (char)ACTION_WIPE;
      ble_send((byte *)&ble_tx_data, sizeof(ble_tx_data));
    }
}

static void emg_task() {

  int           emg = 0;
  SENSORS_DATA  sensors;
  unsigned long micros_start = 0;

  // change thread priority
  osThreadSetPriority(osThreadGetId(), osPriorityNormal);

  // EMG Sampling
  emg = emg_read(A0);
  if (emg > dbg_max) {
    dbg_max = emg;
  }
  // IMU Sampling
  // T.B.D
//  imu_read(&sensors);

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

  buf_counter++;
  if(buf_counter >= EMG_SAMPLES){
    buf_counter = 0;
    grp_counter++;

    // Inference and BLE TX
//    if (dbg_max >= 4) {
    if ((grp_counter % 4) == 0) {
      // set event flag
      event_flags.set(ACTION_TOUCH_FLAG);
    }
  }
}

void setup() {
  unsigned int  emg_sampling_period_us;

  // initialize serial
  Serial.begin(115200);
  while (!Serial);

  // set gloval variables
  emg_sampling_period_us = round(1000000 * (1.0/EMG_SAMPLING_FREQUENCY));
  Serial.print("EMG Sampling Period:");
  Serial.println(emg_sampling_period_us);

  // setup each device
  emg_setup();
  imu_setup();

  ble_setup();

  // create ble thread
  thread_touch.start(ble_thread_touch);
  thread_wipe.start(ble_thread_wipe);

  // set emg timer interval
  timer_emg.setInterval(emg_sampling_period_us/1000, emg_task);
  // set imu timer interval
  // TBD
}

void loop() {
  timer_emg.run();
}
