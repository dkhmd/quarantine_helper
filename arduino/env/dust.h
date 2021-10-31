#ifndef __dust_h__
#define __dust_h__

#include <Arduino.h>

void dust_setup(int pin);
float dust_read(int pin, unsigned long sampletime_ms);

#endif
