import serial
import matplotlib.pyplot as plt
import numpy as np

SAMPLES = 1024
FFT_SAMPLES = int(SAMPLES/2)

arr = [0] * FFT_SAMPLES
x = range(0, FFT_SAMPLES)
fig,ax = plt.subplots()

ser = serial.Serial('/dev/tty.usbmodem1141301', 115200, timeout=None)

while True:
    for i in range(0, FFT_SAMPLES):
        val = float(ser.readline().decode().replace('\r', '').replace('\n', ''))
        arr[i] = val

    line, = ax.plot(x, arr, color='blue')
    plt.pause(0.1)
    line.remove()

ser.close()