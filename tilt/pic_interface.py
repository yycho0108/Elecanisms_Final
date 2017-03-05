import numpy as np
import usb.core

def cap(mn,x,mx):
    return max(mn,min(mx,x))

class PICInterface(object):
    def __init__(self):
        self.WRITE_X = 1
        self.WRITE_Y = 2
        self.TOGGLE_LED = 3

        # self.READ_ACC = 3 ...

        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        self.dev.set_configuration()

    def close(self):
        self.dev = None

    def toggle_led(self):
        try:
            self.dev.ctrl_transfer(0x40, self.TOGGLE_LED1)
        except usb.core.USBError:
            print "Could not send TOGGLE_LED1 vendor request."

    def write_x(self, x):
        try:
            # x = -90 ~ 90
            x = (x + 90)/ 180.
            x = cap(0.,x,1.) # limit
            x = np.uint16(x * 0xFFFF)
            self.dev.ctrl_transfer(0xC0, self.WRITE_X, 0, x, 0)
        except usb.core.USBError as e:
            print "Could not send WRITE_X vendor request."

    def write_y(self, y):
        try:
            y = (y + 90)/ 180.
            y = cap(0.,y,1.) # limit
            y = np.uint16(y * 0xFFFF)
            self.dev.ctrl_transfer(0xC0, self.WRITE_Y, 0, y, 0)
        except usb.core.USBError as e:
            print e
            print "Could not send WRITE_Y vendor request."
