#include "imu.h"
#include <Arduino_LSM9DS1.h>

SENSOR_DATA sensors[SENSOR_KIND_MAX];

void setup_imu() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

SENSOR_DATA *loop_imu() {
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(sensors[SENSOR_KIND_ACC].x, sensors[SENSOR_KIND_ACC].y, sensors[SENSOR_KIND_ACC].z); 
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(sensors[SENSOR_KIND_GYRO].x, sensors[SENSOR_KIND_GYRO].y, sensors[SENSOR_KIND_GYRO].z); 
  }

  if (IMU.magneticFieldAvailable()) {
    IMU.readMagneticField(sensors[SENSOR_KIND_MAG].x, sensors[SENSOR_KIND_MAG].y, sensors[SENSOR_KIND_MAG].z); 
  }

  return sensors;
}
