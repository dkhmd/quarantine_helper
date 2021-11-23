#include <mbed.h>
#include <rtos.h>
#include <NRF52_MBED_TimerInterrupt.h>

#include "common.h"
#include "emg.h"
#include "imu.h"
#include "fft.h"
#include "ble_peripheral.h"
#include "inference.h"


/*** Definition ***/
// #define __COLLECT_RAW_DATA__    true

#define SAMPLES                     256 //Must be a power of 2
#define SAMPLING_FREQUENCY          200 //Hz, must be less than 10000 due to ADC

#define SENSOR_FLAG_TMR_AVAILABLE   (1UL << 0)
#define SENSOR_FLAG_BLE_AVAILABLE   (1UL << 1)

#define INF_FLAG_DATA_A             (1UL << 0)
#define INF_FLAG_DATA_B             (1UL << 1)

#define BLE_FLAG_TOUCH              (1UL << 0)
#define BLE_FLAG_WIPE               (1UL << 1)

#define DBG_BUFFER_SIZE             256
#define MAX_BEACON_NUM              8

#define PORT_BUZZER                 9


/*** Name Space ***/
using namespace rtos;


/*** Structure ***/


/*** Variables ***/
int               sensor_counter = 0;
float             emg_data[SAMPLES];
SENSORS_DATA      imu_data[SAMPLES];

int               grp_counter = 0;
int               buf_counter = 0;
Thread            sensor_thread, inference_a_thread, inference_b_thread, ble_touch_thread, ble_wipe_thread;
EventFlags        sensor_flags, inf_flags, ble_flags;

char              dbg_buf[DBG_BUFFER_SIZE];
char              dbg_max = 0;

// Init NRF52 timer NRF_TIMER3
NRF52_MBED_Timer  ITimer0(NRF_TIMER_3);


/*** Interrupt ***/
void timer_handler()
{  
  sensor_flags.set(SENSOR_FLAG_TMR_AVAILABLE);
}

/*** Functions ***/
static void transmit_ble(ACTION act){
  // BLE Transmit
  switch (act){
    case ACTION_TOUCH:
      sensor_flags.clear(SENSOR_FLAG_BLE_AVAILABLE);  // stop sensor
      ble_flags.set(BLE_FLAG_TOUCH);                  // start ble thread
      break;
    case ACTION_WIPE:
      sensor_flags.clear(SENSOR_FLAG_BLE_AVAILABLE);  // stop sensor
      ble_flags.set(BLE_FLAG_WIPE);                   // start ble thread
      break;
    case ACTION_NONE:
    default:
      break;
  }
}

static void ble_touch_thread_cb(){
  while(true) {
    ble_flags.wait_all(BLE_FLAG_TOUCH);
  
    Serial.println("[BLE]Notify TOUCH");
    ble_peripheral_notify(ACTION_TOUCH);

    sensor_counter = 0;
    sensor_flags.set(SENSOR_FLAG_BLE_AVAILABLE);
  }
}

static void ble_wipe_thread_cb(){
  while(true) {
    ble_flags.wait_all(BLE_FLAG_WIPE);

    Serial.println("[BLE]Notify WIPE");
    ble_peripheral_notify(ACTION_WIPE);

    sensor_counter = 0;
    sensor_flags.set(SENSOR_FLAG_BLE_AVAILABLE);
  }
}

static void inference_a_thread_cb(){
  osThreadSetPriority(osThreadGetId(), osPriorityAboveNormal);
  while(true) {
    ACTION act = ACTION_NONE;
    
    inf_flags.wait_all(INF_FLAG_DATA_A, osWaitForever, false);

    // Inference
    inference_exec(&emg_data[0], SAMPLES, &act);
    transmit_ble(act);

    // clear flag
    inf_flags.clear(INF_FLAG_DATA_A);
  }
}

static void inference_b_thread_cb(){
  osThreadSetPriority(osThreadGetId(), osPriorityAboveNormal);
  while(true) {
    ACTION act = ACTION_NONE;

    inf_flags.wait_all(INF_FLAG_DATA_B, osWaitForever, false);

    // Inference
    inference_exec(&emg_data[SAMPLES], SAMPLES, &act);
    transmit_ble(act);

    // clear flag
    inf_flags.clear(INF_FLAG_DATA_B);
  }
}

static void sensor_thread_cb() {
  osThreadSetPriority(osThreadGetId(), osPriorityRealtime);
  
  while(true){
    sensor_flags.wait_all(SENSOR_FLAG_TMR_AVAILABLE | SENSOR_FLAG_BLE_AVAILABLE, osWaitForever, false);
    if((sensor_counter == 0) || (sensor_counter == SAMPLES)){
      Serial.println("[SENSOR]Scan start");
    }

    // EMG Sampling(200HZ)
    emg_data[sensor_counter] = emg_read(A0);
  
    // IMU Sampling(100HZ)
    if((sensor_counter % 2) == 0){
      imu_read(&imu_data[sensor_counter % 2]);
    }

  #ifdef __COLLECT_RAW_DATA__
    if((grp_counter % 4) == 0) {
        digitalWrite(PORT_BUZZER, LOW);
    } else if(((grp_counter % 4) == 3) || (sensor_counter <= SAMPLES/4)) {
        digitalWrite(PORT_BUZZER, HIGH);
    } else {
        digitalWrite(PORT_BUZZER, LOW);
    }
  
    if((grp_counter % 4) == 0) {
      // print sensor values
      sprintf(dbg_buf, "[DATA]grp:%d, cnt:%d, sample:%d, emg:%f, acc_x:%f, acc_y:%f, acc_z:%f, gyro_x:%f, gyro_y:%f, gyro_z:%f", \
                    grp_counter, buf_counter, SAMPLES, emg_data[sensor_counter],
                    imu_data[sensor_counter].sensor[SENSOR_KIND_ACC].x, imu_data[sensor_counter].sensor[SENSOR_KIND_ACC].y, imu_data[sensor_counter].sensor[SENSOR_KIND_ACC].z, \
                    imu_data[sensor_counter].sensor[SENSOR_KIND_GYRO].x, imu_data[sensor_counter].sensor[SENSOR_KIND_GYRO].y, imu_data[sensor_counter].sensor[SENSOR_KIND_GYRO].z);
      Serial.println(dbg_buf);
    }
  #endif
  
    sensor_counter++;
    if(sensor_counter == SAMPLES){
      Serial.println("[SENSOR]Scan a done");
      inf_flags.set(INF_FLAG_DATA_A);  
    } else if(sensor_counter == SAMPLES * 2){
      Serial.println("[SENSOR]Scan b done");
      inf_flags.set(INF_FLAG_DATA_B);
      sensor_counter = 0;
    }
    sensor_flags.clear(SENSOR_FLAG_TMR_AVAILABLE);
  }
}

void setup() {
  unsigned int  sampling_period_us = 0;

  digitalWrite(LED_PWR, LOW);

  // initialize serial
  Serial.begin(115200);
//  while (!Serial);

  // set gloval variables
  sampling_period_us = round(1000000 * (1.0/SAMPLING_FREQUENCY));
  Serial.print("Sampling Period:");
  Serial.println(sampling_period_us);

  // setup each device
  emg_setup();
  imu_setup();
  ble_peripheral_setup();
  inference_setup();

  // create ble thread
  ble_touch_thread.start(ble_touch_thread_cb);
  ble_wipe_thread.start(ble_wipe_thread_cb);

  // create sensor and inference thread
  sensor_thread.start(sensor_thread_cb);
  inference_a_thread.start(inference_a_thread_cb);
  inference_b_thread.start(inference_b_thread_cb);

  // set sensor flag
  sensor_flags.clear(SENSOR_FLAG_TMR_AVAILABLE);
  sensor_flags.set(SENSOR_FLAG_BLE_AVAILABLE);

  // start timer
  if (!(ITimer0.attachInterruptInterval(sampling_period_us, timer_handler))){
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));
  }
}

void loop() {
  ble_peripheral_loop();
}
