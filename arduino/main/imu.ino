#include <Arduino_LSM9DS1.h>

void setup_imu() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void loop_imu() {
  float x, y, z;

  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(x, y, z); 
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(x, y, z);
  }

  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(x, y, z);
  }
}
