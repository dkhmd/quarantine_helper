#include "co2_ndir.h"

int prevVal = LOW;
long th, tl, h, l, ppm;

byte ReadCO2[9] = {0xFF, 0x01, 0x86, 0x00, 0x00, 0x00, 0x00, 0x00, 0x79};
byte SelfCalOn[9]  = {0xFF, 0x01, 0x79, 0xA0, 0x00, 0x00, 0x00, 0x00, 0xE6};
byte SelfCalOff[9] = {0xFF, 0x01, 0x79, 0x00, 0x00, 0x00, 0x00, 0x00, 0x86};
byte retval[9];

void co2_ndir_setup() {  
  Serial1.begin(9600);
  while (!Serial1);

  Serial1.write(SelfCalOn, sizeof(SelfCalOn));
  Serial1.readBytes((char *)retval, sizeof retval);
}
 
bool co2_ndir_read(uint16_t *co2) {
  if (Serial1.available() < 0) {
    return false;
  }

  Serial1.write(ReadCO2, sizeof(ReadCO2));
  Serial1.readBytes((char *)retval, sizeof(retval));
  if ((retval[0] == 0xFF) && (retval[1] == 0x86)) {
    *co2 = retval[2] * 256 + retval[3];
    return true;
  }

  return false;
}
