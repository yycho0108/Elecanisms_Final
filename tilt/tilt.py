#!/usr/bin/python

from wiiboard import Wiiboard, EventProcessor
from pic_interface import PICInterface
from get_angle import tilt2servo
from get_ip import get_ip 

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

coin_sound = pygame.mixer.Sound("sound/coin.wav")
coin_sound_length = f_sound.get_length()
coin_sound_last = 0

def time_now():
    return time.time()

def read_cmd(ser):
    global e_sound_last, f_sound_last, c_sound_last, coin_sound_last
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
                elif 'coin' in sfx_type:
                    if (now - coin_sound_last) > coin_sound_length:
                        coin_sound.play()
                        coin_sound_last = now
            else:
                print cmd
        except Exception as e:
            print e
            pass
        time.sleep(0.04)
#

#def main_v2():
#    processor = EventProcessor()
#    pic = PICInterface()
#    board = Wiiboard(processor, disabled = False)
#    while True:
#         if pic.connected:
#               pic.write_ip(get_ip('wlan0'))
#               if board.isConnected():
#                   pass
#           else:
#               pic.connect()
#       while True:
#           if pic.connected:



def main():
    processor = EventProcessor()
    board = Wiiboard(processor, disabled = False)

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
        pic.write_ip(get_ip('wlan0'))

        print 'connect to wii board!'
        board.connect()
        if board.isConnected():
            board.async_receive()

        while True:
            if pic.connected:
                t_x, t_y = processor.t_x, processor.t_y # tilt angles
                t_x, t_y = -t_y, t_x # this is flipped due to servo position
                #s_x, s_y = tilt2servo(t_x, rad=False), tilt2servo(t_y, rad=False) # servo angles
                s_x, s_y = 6 * t_x, 6 * t_y # 15 -> 90

                wii = board.isConnected()
                if not (pic.write_x(s_x) and pic.write_y(s_y) and pic.write_wii(wii)):
                    pic.connected = False
                if not wii: # try to connect to wii again
                    board.connect() # try connecting again
                    if board.isConnected():
                        board.setLight(False)
                        board.wait(500)
                        board.setLight(True)
                        board.async_receive()
                    
            else:
                print 'pic not connected'
                pic.connect()
                pic.write_ip(get_ip('wlan0'))

            time.sleep(0.05)

        pic.close()




if __name__ == "__main__":
    main()
