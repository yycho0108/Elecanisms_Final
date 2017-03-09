from wiiboard import Wiiboard, EventProcessor
from pic_interface import PICInterface
from get_angle import tilt2servo

import serial
import time
import threading

def read_ser(ser):
    while ser.isOpen():
        s = '>' + ser.readline()
        print s 
        time.sleep(0.04)

def main():
    processor = EventProcessor()
    board = Wiiboard(processor)

    print "Trying to connect..."
    board.connect()  # The wii board must be in sync mode at this time
    board.wait(200)

    # Flash the LED so we know we can step on.
    board.setLight(False)
    board.wait(500)
    board.setLight(True)
    board.async_receive()

    ## Communicate via UART - debugging
    #with serial.Serial(
    #        port = '/dev/ttyAMA0',
    #        baudrate = 19200,
    #        parity = serial.PARITY_NONE,
    #        stopbits=serial.STOPBITS_ONE,
    #        timeout=None) as ser:

    #    r_t = threading.Thread(target = read_ser, args=(ser,))
    #    r_t.daemon = True
    #    r_t.start()

    pic = PICInterface()

    while True:
        t_x, t_y = processor.t_x, processor.t_y # tilt angles
        #t_x, t_y = 0,10 # for testing
        t_x, t_y = -t_y, t_x # this is flipped due to servo position
        s_x, s_y = tilt2servo(t_x, rad=False), tilt2servo(t_y, rad=False) # servo angles

        print 'writing tilt : ({0:.2f}, {1:.2f}); servo : ({2:.2f},{3:.2f})'.format(t_x, t_y, s_x, s_y)
        pic.write_x(s_x)
        pic.write_y(s_y)
        time.sleep(0.05)

    pic.close()

if __name__ == "__main__":
    main()
