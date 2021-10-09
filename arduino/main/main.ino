#include "emg.h"
#include "imu.h"
#include "ble_master.h"

void setup() {
  Serial.begin(115200);
  while (!Serial);

  setup_emg();
  setup_imu();

  setup_ble_master();
}
 
void loop() {
  loop_emg(A0);
  loop_imu();

  loop_ble_master();

  delay(1000);
}
