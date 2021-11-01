#include <Wire.h>
#include <mbed.h>
#include <rtos.h>
#include <NRF52_MBED_TimerInterrupt.h>

#include "temperature.h"
#include "color.h"
#include "voc.h"
#include "co2_pas.h"
#include "co2_ndir.h"
#include "dust.h"
#include "ble_peripheral.h"

#define dustPin             11
#define SAMPLING_PERIOD_US  (1 * 1000 * 1000)
#define SAMPLING_TIMES      30

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
  osThreadSetPriority(osThreadGetId(), osPriorityNormal);

  while(true){
    tmr_flags.wait_all(FLAG_TMR_AVAILABLE, osWaitForever, false);
    
    int color_r = 0, color_g = 0, color_b = 0, color_a = 0;
    static int prev_color_r = 0, prev_color_g = 0, prev_color_b = 0, prev_color_a = 0;
  
    float temperature = 0, humidity = 0;
    static float prev_temperature = 0, prev_humidity = 0;
  
    uint16_t voc = 0;
    static uint16_t prev_voc = 0;
  
    uint16_t co2_pas = 0;
    static uint16_t prev_co2_pas = 0;
  
    float co2_pas_temperature = 0, co2_pas_humidity = 0;
    static float prev_co2_pas_temperature = 0, prev_co2_pas_humidity = 0;
  
    uint16_t co2_ndir = 0;
    static uint16_t prev_co2_ndir = 0;
  
    double dust = 0;
    static double prev_dust = 0;

    sampling_cnt++;
    if (sampling_cnt < SAMPLING_TIMES){
        voc_read(&voc);
        tmr_flags.clear(FLAG_TMR_AVAILABLE);
        continue;
    }else {
      sampling_cnt = 0;
    }
  
  //  // Temperature and Humidity
  //  if(temperature_read(&temperature, &humidity) != true) {
  //    temperature = prev_temperature; humidity = prev_humidity;
  //  }
  //  Serial.print("temp:");
  //  Serial.print(temperature);
  //  Serial.print(", hum:");
  //  Serial.print(humidity);
  //  prev_temperature = temperature; prev_humidity = humidity;
  
    // Color
    if(color_read(&color_r, &color_g, &color_b, &color_a) == true) {
      ble_peripheral_notify_color(color_a);
    } else {
      color_r = prev_color_r; color_g = prev_color_g; color_b = prev_color_b; color_a = prev_color_a;
    }
    Serial.print("color_a:");
    Serial.print(color_a);
//    Serial.print(", color_r:");
//    Serial.print(color_r);
//    Serial.print(", color_g:");
//    Serial.print(color_g);
//    Serial.print(", color_b:");
//    Serial.print(color_b);
    prev_color_r = color_r; prev_color_g = color_g; prev_color_b = color_b; prev_color_a = color_a;
  
    // VOC
    if(voc_read(&voc) == true) {
      ble_peripheral_notify_voc(voc);
    } else {
      voc = prev_voc;
    }
    Serial.print(", voc:");
    Serial.print(voc);
    prev_voc = voc;
  
    // CO2(PAS)
    if(co2_pas_read(&co2_pas, &co2_pas_temperature, &co2_pas_humidity) == true) {
      ble_peripheral_notify_temperature(co2_pas_temperature);
      ble_peripheral_notify_humidity(co2_pas_humidity);
      ble_peripheral_notify_co2_pas(co2_pas);
    } else {
      co2_pas = prev_co2_pas; co2_pas_temperature = prev_co2_pas_temperature; co2_pas_humidity = prev_co2_pas_humidity;
    }
    Serial.print(", co2(pas):");
    Serial.print(co2_pas);
    Serial.print(", temp:");
    Serial.print(co2_pas_temperature);
    Serial.print(", hum:");
    Serial.print(co2_pas_humidity);
    prev_co2_pas = co2_pas; prev_co2_pas_temperature = co2_pas_temperature; prev_co2_pas_humidity = co2_pas_humidity;
  
    // CO2(NDIR)
    if(co2_ndir_read(&co2_ndir) == true) {
      ble_peripheral_notify_co2_ndir(co2_ndir);
    } else {
      co2_ndir = prev_co2_ndir;
    }
    Serial.print(", co2(ndir):");
    Serial.print(co2_ndir);
    prev_co2_ndir = co2_ndir;
  
    // DUST
//    if(dust_read(dustPin, SAMPLING_PERIOD_US/1000, &dust) == true) {
//      ble_peripheral_notify_dust(dust);
//    } else {
//      dust = prev_dust;
//    }
//    Serial.print(", dust:");
//    Serial.print(dust);
//    prev_dust = dust;
  
    Serial.println("");
    tmr_flags.clear(FLAG_TMR_AVAILABLE);
  }
}

void setup() {
  digitalWrite(LED_PWR, LOW);
  
  // initialize serial
  Serial.begin(115200);
  while (!Serial);

  // initialize i2c
  Wire.begin();

  // setup each device
  temperature_setup();
  color_setup();
  voc_setup();
  co2_pas_setup();
  co2_ndir_setup();
  dust_setup(dustPin);

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
