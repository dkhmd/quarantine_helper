#include "temperature.h"

void setup() {
  digitalWrite(LED_PWR, LOW);
  
  // initialize serial
  Serial.begin(115200);
  while (!Serial);

  // setup each device
  temperature_setup();

}

void loop() {
  float temperature, humidity;
  
  temperature_read(&temperature, &humidity);
  Serial.print("Temperature:");
  Serial.print(temperature);
  
  Serial.print(", Humidity:");
  Serial.print(humidity);
  Serial.println("");

  delay(1000);
}
