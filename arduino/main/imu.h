#ifndef __imu_h__
#define __imu_h__

#include <ArduinoBLE.h>

typedef enum {
  SENSOR_KIND_ACC   = 0,
  SENSOR_KIND_GYRO  = 1,
  SENSOR_KIND_MAG   = 2,
  SENSOR_KIND_MAX
} SENSOR_KIND;

typedef struct {
  float       x;
  float       y;
  float       z;
} SENSOR_DATA;

typedef struct {
  SENSOR_DATA sensor[SENSOR_KIND_MAX];
} SENSORS_DATA;

void setup_imu();
void read_imu(SENSORS_DATA *sensors);

#endif
