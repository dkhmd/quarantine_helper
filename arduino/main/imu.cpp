#include "imu.h"

void setup_imu() {
  if (!IMU.begin()) {
    Serial.println("Failed to initialize IMU!");
    while (1);
  }
}

void read_imu(SENSORS_DATA *sensors) {
  
  if (IMU.accelerationAvailable()) {
    IMU.readAcceleration(sensors->sensor[SENSOR_KIND_ACC].x, sensors->sensor[SENSOR_KIND_ACC].y, sensors->sensor[SENSOR_KIND_ACC].z); 
  }

  if (IMU.gyroscopeAvailable()) {
    IMU.readGyroscope(sensors->sensor[SENSOR_KIND_GYRO].x, sensors->sensor[SENSOR_KIND_GYRO].y, sensors->sensor[SENSOR_KIND_GYRO].z); 
  }

//  if (IMU.magneticFieldAvailable()) {
//    IMU.readMagneticField(sensors->sensor[SENSOR_KIND_MAG].x, sensors->sensor[SENSOR_KIND_MAG].y, sensors->sensor[SENSOR_KIND_MAG].z); 
//  }
}
