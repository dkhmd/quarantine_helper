#ifndef __common_h__
#define __common_h__

#include <Arduino.h>

typedef enum {
  ACTION_NONE   = 0,
  ACTION_TOUCH,
  ACTION_WIPE,
  ACTION_MAX
} ACTION;

#endif
