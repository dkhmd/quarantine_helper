#include <Wire.h>
#include <mbed.h>
#include <rtos.h>
#include <NRF52_MBED_TimerInterrupt.h>

#include "bme280.h"
#include "co2_ndir.h"
#include "ble_peripheral.h"


#define SAMPLING_PERIOD_US  (1 * 1000 * 1000)
#define SAMPLING_TIMES      60

#define FLAG_TMR_AVAILABLE   (1UL << 0)

/*** Name Space ***/
using namespace rtos;


/*** Structure ***/


/*** Variables ***/
int               sampling_cnt = 0;
Thread            sensor_thread;
EventFlags        tmr_flags;

// Init NRF52 timer NRF_TIMER3
NRF52_MBED_Timer  ITimer0(NRF_TIMER_3);


/*** Interrupt ***/
void timer_handler()
{  
  tmr_flags.set(FLAG_TMR_AVAILABLE);
}

/*** Functions ***/
static void sensor_thread_cb() {
  osThreadSetPriority(osThreadGetId(), osPriorityAboveNormal);

  while(true){
    tmr_flags.wait_all(FLAG_TMR_AVAILABLE, osWaitForever, false);
  
    uint16_t co2_ndir = 0;
    static uint16_t prev_co2_ndir = 0;
    float temp = 0, hum = 0, pressure = 0;

    sampling_cnt++;
    if (sampling_cnt < SAMPLING_TIMES){
        // co2(ndir)
        if((sampling_cnt % 2) == 0) {
          co2_ndir_read(&prev_co2_ndir);
        }
        continue;
    } else {
      sampling_cnt = 0;
    }

    // BM280
    bm280_read(&temp, &hum, &pressure);

    // CO2(NDIR)
    if(co2_ndir_read(&co2_ndir) == true) {
      ble_peripheral_notify_co2_ndir(co2_ndir);
    } else {
      co2_ndir = prev_co2_ndir;
    }
    Serial.print(", co2(ndir):");
    Serial.print(co2_ndir);
    prev_co2_ndir = co2_ndir;

  
    Serial.println("");
    tmr_flags.clear(FLAG_TMR_AVAILABLE);
  }
}

void setup() {
  digitalWrite(LED_PWR, LOW);
  
  // initialize serial
  Serial.begin(115200);
//  while (!Serial);

  Serial1.begin(9600);
  while (!Serial1);

  Wire.begin();

  // setup each device
  bme280_setup();
  co2_ndir_setup();

  // ble
  ble_peripheral_setup();

  // create sensor thread
  sensor_thread.start(sensor_thread_cb);  

  // set flag
  tmr_flags.clear(FLAG_TMR_AVAILABLE);

  // start timer
  if (!(ITimer0.attachInterruptInterval(SAMPLING_PERIOD_US, timer_handler))){
    Serial.println(F("Can't set ITimer0. Select another freq. or timer"));
  }
}

void loop() {  
  ble_peripheral_loop();
}
