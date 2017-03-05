import serial

if __name__ == "__main__":
    with serial.Serial(
            port = '/dev/ttyUSB0',
            baudrate = 19200,
            parity = serial.PARITY_NONE,
            stopbits=serial.STOPBITS_ONE,
            timeout=None) as ser:
        while ser.isOpen():
            print ser.readline()   # read a '\n' terminated line
