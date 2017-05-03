import numpy as np
import usb.core

def cap(mn,x,mx):
    return max(mn,min(mx,x))

class PICInterface(object):
    def __init__(self):
        self.WRITE_X = 1
        self.WRITE_Y = 2
        self.WRITE_IP = 3
        self.WRITE_WII = 4
        # self.READ_ACC = 3 ...

        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)
        if self.dev is None:
            raise ValueError('no USB device found matching idVendor = 0x6666 and idProduct = 0x0003')
        #self.dev.set_configuration()

        self.connected = True

    def connect(self):
        self.dev = usb.core.find(idVendor = 0x6666, idProduct = 0x0003)

        if self.dev is None:
            self.connected = False
        else:
            self.connected = True

        return self.connected

    def close(self):
        self.dev = None
        self.connected = False

    def write_x(self, x):
        try:
            # x = -90 ~ 90
            x = (x + 90)/ 180.
            x = cap(0.,x,1.) # limit
            x = np.uint16(x * 0xFFFF)
            self.dev.ctrl_transfer(0xC0, self.WRITE_X, 0, x, 0)
            return True
        except Exception as e:
            print e
            print "Could not send WRITE_X vendor request."
            return False

    def write_y(self, y):
        try:
            y = (y + 90)/ 180.
            y = cap(0.,y,1.) # limit
            y = np.uint16(y * 0xFFFF)
            self.dev.ctrl_transfer(0xC0, self.WRITE_Y, 0, y, 0)
            return True
        except Exception as e:
            print e
            print "Could not send WRITE_Y vendor request."
            return False

    def write_ip(self, ip):
        try:
            ip = [int(x) for x in ip.split('.')]
            ip_high = np.uint16(ip[0]*256 + ip[1])
            ip_low = np.uint16(ip[2]*256 + ip[3])
            self.dev.ctrl_transfer(0xC0, self.WRITE_IP, ip_high, ip_low, 0)
            return True
        except Exception as e:
            print e
            print "Could not send WRITE_IP vendor request."
            return False

    def write_wii(self, wii):
        try:
            wii = 0xFFFF if wii else 0x0000
            self.dev.ctrl_transfer(0xC0, self.WRITE_WII, 0, wii, 0)
            return True
        except Exception as e:
            print e
            print "Could not send WRITE_WII vendor request."
            return False
