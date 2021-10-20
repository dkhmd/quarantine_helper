#include "fft.h"

#define MAX_SAMPLES 256

arduinoFFT FFT = arduinoFFT();
   
void calc_fft(double input[], int freq, int samples, double output[], double *peak) {
  double vImag[MAX_SAMPLES];

  memcpy(output, input, sizeof(double) * samples);
  memset(vImag, 0x00, sizeof(vImag));

  /*FFT*/
  FFT.Windowing(output, samples, FFT_WIN_TYP_HAMMING, FFT_FORWARD);
  FFT.Compute(output, vImag, samples, FFT_FORWARD);
  FFT.ComplexToMagnitude(output, vImag, samples);
  *peak = FFT.MajorPeak(output, samples, freq); 
}
