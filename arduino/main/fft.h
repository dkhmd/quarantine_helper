#ifndef __fft_h__
#define __fft_h__

#include <arduinoFFT.h>

void calc_fft(double input[], int freq, int samples, double output[], double *peak);

#endif
