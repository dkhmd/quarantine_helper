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

#define EMG_FLAG                (1UL << 0)
#define IMU_FLAG                (1UL << 1)

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

int           emg_data[EMG_SAMPLES];
int           emg_counter = 0;
SENSORS_DATA  imu_data[IMU_SAMPLES];
int           imu_counter = 0;


int           grp_counter = 0;
int           buf_counter = 0;
SimpleTimer   timer_emg, timer_imu;
Thread        thread_inference;
EventFlags    inf_flags;

char          dbg_buf[DBG_BUFFER_SIZE];
char          dbg_max = 0;


static void inference_thread(){  
  while(true) {
    TX_DATA       ble_tx_data;

    inf_flags.wait_all(EMG_FLAG | IMU_FLAG);

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
#else
    // BLE Transmit
    memset(&ble_tx_data, 0x00, sizeof(ble_tx_data));
    Serial.print("BLE TX Data Size:");
    Serial.println(sizeof(ble_tx_data));

    ble_get_address((char *)ble_tx_data.addr);   
    ble_tx_data.action = (char)ACTION_TOUCH;
    ble_send((byte *)&ble_tx_data, sizeof(ble_tx_data));
#endif

    // clear flag
    inf_flags.clear(EMG_FLAG | IMU_FLAG);

    // clear counter
    emg_counter = 0;
    imu_counter = 0;
  }
}

static void emg_task() {
  if(emg_counter == EMG_SAMPLES){
    return;
  }

  // EMG Sampling
  emg_data[emg_counter] = emg_read(A0);
  emg_counter++;

  if(emg_counter == EMG_SAMPLES){
    inf_flags.set(EMG_FLAG);
  }
}

static void imu_task() {
  if(imu_counter == IMU_SAMPLES){
    return;
  }

  imu_read(&imu_data[imu_counter]);
  imu_counter++;

  if(imu_counter == IMU_SAMPLES){
    inf_flags.set(IMU_FLAG);
  }
}

void setup() {
  unsigned int  emg_sampling_period_us, imu_sampling_period_us;

  digitalWrite(LED_PWR, LOW);

  // initialize serial
  Serial.begin(115200);
  while (!Serial);

  // set gloval variables
  emg_sampling_period_us = round(1000000 * (1.0/EMG_SAMPLING_FREQUENCY));
  imu_sampling_period_us = round(1000000 * (1.0/IMU_SAMPLING_FREQUENCY));
  Serial.print("EMG Sampling Period:");
  Serial.println(emg_sampling_period_us);

  // setup each device
  emg_setup();
  imu_setup();

  ble_setup();

  // create inference thread
  thread_inference.start(inference_thread);

  // set emg timer interval
  timer_emg.setInterval(emg_sampling_period_us/1000, emg_task);
  // set imu timer interval
  timer_emg.setInterval(imu_sampling_period_us/1000, imu_task);
}

void loop() {
  timer_emg.run();
  timer_imu.run();
}
