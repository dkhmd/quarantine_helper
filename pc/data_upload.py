import sys
import serial
import matplotlib.pyplot as plt

def main():
    if len(sys.argv) == 1:
        SAMPLES = 256
    elif len(sys.argv) == 2:
        SAMPLES = int(sys.argv[1])
    else:
        print("usage: python data_serial.py (samples)")
        exit(0)

    print("SAMPLE = " + str(SAMPLES))

    fig, ax = plt.subplots()

    arr = [0] * SAMPLES
    x = range(0, SAMPLES)

    ser = serial.Serial('/dev/tty.usbmodem1141301', 115200, timeout=None)
    try:
        while True:
            for i in range(0, SAMPLES):
                val = float(ser.readline().decode().replace('\r', '').replace('\n', ''))
                arr[i] = val

            ax.set_xlim(0, SAMPLES)
            ax.set_ylim(400, 1024)
            line, = ax.plot(x, arr, color='blue')
            plt.pause(0.1)
            line.remove()
    except:
        ser.close()

if __name__ == '__main__':
    main()
