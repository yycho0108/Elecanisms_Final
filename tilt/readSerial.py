#!/usr/bin/python

import serial

with serial.Serial(
       port = '/dev/ttyUSB0',
       baudrate = 19200,
       parity = serial.PARITY_NONE,
       stopbits=serial.STOPBITS_ONE,
       timeout=None) as ser:
	while ser.isOpen():
		print ser.readline()
