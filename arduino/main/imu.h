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
  public:
    float x;
    float y;
    float z;
} SENSOR_DATA;


void setup_imu();
SENSOR_DATA *loop_imu();

#endif
