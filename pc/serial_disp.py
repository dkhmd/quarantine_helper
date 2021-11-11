import serial

SERIAL_DEVICE_NAME = "/dev/tty.usbmodem1141301"
def run():
    ser = serial.Serial(SERIAL_DEVICE_NAME, 115200, timeout=None)
    while True:
       line = ser.readline().decode().replace('\r', '').replace('\n', '')
       print(line)
    ser.close()

if __name__ == '__main__':
	run()
