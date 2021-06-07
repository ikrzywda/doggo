#!/usr/bin/python

'''Module for interfacing with doggo Arduino module:
    - refreshing date and time upon connection
    - ability to dump SD file data without taking out the SD'''

from sys import platform
from time import localtime, sleep
import serial

def init_serial():
    '''Initializes communication with the module at 115200 baud-rate'''
    port_name = ''
    if platform.startswith('linux'):
        port_name = '/dev/ttyACM0'
    elif platform.startswith('win32'):
        port_name = 'COM7'

    return serial.Serial(port_name, 115200, timeout=None)

def dump_sd(port):
    port.write(b'd')
    f = open('temp_.txt', 'w')
    port = init_serial()
    while (c := port.read().decode('ASCII')) != '|':
        print(c, end='')
        f.write(c)
    f.close()

def update_time(port):
    port.write(b't')
    port.write(localtime().tm_hour.to_bytes(1,'big'))
    port.write(localtime().tm_min.to_bytes(1, 'big'))
    port.write(localtime().tm_sec.to_bytes(1, 'big'))
    port.write(localtime().tm_mday.to_bytes(1, 'big'))
    port.write(localtime().tm_mon.to_bytes(1, 'big'))
    port.write((localtime().tm_year % 2000).to_bytes(1, 'big'))

def cli(): 
    port = init_serial()
    while (cmd := input('>>>')) != 'q':
        if cmd == 't':
            update_time(port)
        elif cmd == 'd':
            dump_sd(port)
        else:
            while port.in_waiting:
                print(port.readline())


if __name__ == '__main__':
    cli()
