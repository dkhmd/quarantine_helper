#ifndef __co2_ndir_h__
#define __co2_ndir_h__

#include <Arduino.h>

void co2_ndir_setup();
bool co2_ndir_read(uint16_t *co2);

#endif
