#!/usr/bin/python

from wiiboard import Wiiboard, EventProcessor
from pic_interface import PICInterface
from get_angle import tilt2servo

import serial
import time
import threading
import pygame

pygame.mixer.init()
pygame.mixer.music.load('sound/background.mp3')
pygame.mixer.music.play(loops=-1, start=0.0)
pygame.mixer.music.set_volume(0.3)

e_sound = pygame.mixer.Sound("sound/electromagnet.wav")
e_sound_length = e_sound.get_length()
e_sound_last = 0

f_sound = pygame.mixer.Sound("sound/swing.wav")
f_sound_length = f_sound.get_length()
f_sound_last = 0


c_sound = pygame.mixer.Sound("sound/confusion.wav")
c_sound_length = f_sound.get_length()
c_sound_last = 0

def time_now():
    return time.time()

def read_cmd(ser):
    global e_sound_last, f_sound_last
    while ser.isOpen():
        try:
            now = time_now()
            cmd = ser.readline()
            l = cmd.split(':')
            if len(l) > 0 and l[0] == 'sfx':
                sfx_type = l[1]
                if 'electromagnet' in sfx_type:
                    if (now - e_sound_last) > e_sound_length:
                        e_sound.play()
                        e_sound_last = now
                elif 'flipper' in sfx_type:
                    if (now - f_sound_last) > f_sound_length:
                        f_sound.play()
                        f_sound_last = now
                elif 'ctrl_flip' in sfx_type:
                    if (now - c_sound_last) > c_sound_length:
                        c_sound.play()
                        c_sound_last = now
            else:
                print cmd
        except Exception as e:
            print e
            pass
        time.sleep(0.04)

def main():
    processor = EventProcessor()
    with Wiiboard(processor, disabled=False) as board:
        #print "Trying to connect..."
        board.wait(200)

        # Flash the LED so we know we can step on.
        board.setLight(False)
        board.wait(500)
        board.setLight(True)
        board.async_receive()

        ## Communicate via UART - debugging
        with serial.Serial(
                port = '/dev/ttyUSB0',
                baudrate = 19200,
                parity = serial.PARITY_NONE,
                stopbits=serial.STOPBITS_ONE,
                timeout=None) as ser:
            r_t = threading.Thread(target = read_cmd, args=(ser,))
            r_t.daemon = True
            r_t.start()

            print 'connect with pic now!'
            pic = PICInterface()

            while True:
                if pic.connected:
                    t_x, t_y = processor.t_x, processor.t_y # tilt angles
                    t_x, t_y = -t_y, t_x # this is flipped due to servo position
                    s_x, s_y = tilt2servo(t_x, rad=False), tilt2servo(t_y, rad=False) # servo angles

                    #print 'writing tilt : ({0:.2f}, {1:.2f}); servo : ({2:.2f},{3:.2f})'.format(t_x, t_y, s_x, s_y)
                    if not (pic.write_x(s_x) and pic.write_y(s_y)):
                        pic.connected = False
                else:
                    print 'pic not connected'
                    pic.connect()

                time.sleep(0.05)

            pic.close()

if __name__ == "__main__":
    main()
