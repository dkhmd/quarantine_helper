#include <Wire.h>

#include "temperature.h"
#include "color.h"
#include "voc.h"
#include "co2_pas.h"
#include "dust.h"

int dustPin = 12;

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
  dust_setup(dustPin);

  delay(5000);
}

void loop() {
  int color_r = 0, color_g = 0, color_b = 0;
  static int prev_color_r = 0, prev_color_g = 0, prev_color_b = 0;

  float temperature = 0, humidity = 0;
  static float prev_temperature = 0, prev_humidity = 0;

  uint16_t voc = 0;
  static uint16_t prev_voc = 0;

  uint16_t co2_pas = 0;
  static uint16_t prev_co2_pas = 0;

  float co2_pas_temperature = 0, co2_pas_humidity = 0;
  static float prev_co2_pas_temperature = 0, prev_co2_pas_humidity = 0;

  double dust = 0;
  static double prev_dust = 0;
//
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
  if(color_read(&color_r, &color_g, &color_b) != true) {
    color_r = prev_color_r; color_g = prev_color_g; color_b = prev_color_b;
  }
  Serial.print("color_r:");
  Serial.print(color_r);
  Serial.print(", color_g:");
  Serial.print(color_g);
  Serial.print(", color_b:");
  Serial.print(color_b);
  prev_color_r = color_r; prev_color_g = color_g; prev_color_b = color_b;

  // VOC
  if(voc_read(&voc) != true) {
    voc = prev_voc;
  }
  Serial.print(", voc:");
  Serial.print(voc);
  prev_voc = voc;

  // CO2(PAS)
  if(co2_pas_read(&co2_pas, &co2_pas_temperature, &co2_pas_humidity) != true) {
    co2_pas = prev_co2_pas; co2_pas_temperature = prev_co2_pas_temperature; co2_pas_humidity = prev_co2_pas_humidity;
  }
  Serial.print(", co2(pas):");
  Serial.print(co2_pas);
  Serial.print(", temp:");
  Serial.print(co2_pas_temperature);
  Serial.print(", hum:");
  Serial.print(co2_pas_humidity);
  prev_co2_pas = co2_pas; prev_co2_pas_temperature = co2_pas_temperature; prev_co2_pas_humidity = co2_pas_humidity;

  // DUST
  if(dust_read(dustPin, &dust) != true) {
    dust = prev_dust;
  }
  Serial.print(", dust:");
  Serial.print(dust);
  prev_dust = dust;

  Serial.println("");
}
