void setup() {
  Serial.begin(115200);
  while (!Serial);

  setup_emg();
  setup_imu();

  setup_ble();
}
 
void loop() {
  loop_emg();
  loop_imu();

  loop_ble();
}
