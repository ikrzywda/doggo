#!/usr/bin/env python

'''Module for interfacing with doggo Arduino module:
    - refreshing date and time upon connection
    - ability to dump SD file data without taking out the SD'''

from sys import platform
from time import localtime
import re
import serial

BLOB = 'temp.txt'   # file where all data from SD is stored
UPDATE_TIME = b't'
DUMP_FILES = b'd'

def init_serial():
    '''Initializes communication with the module at 115200 baud-rate
        and chooses appropriate port for respective platform'''
    port_name = ''
    if platform.startswith('linux'):
        port_name = '/dev/ttyACM0'
    elif platform.startswith('win32'):
        port_name = 'COM7'

    return serial.Serial(port_name, 115200, timeout=None)

def dump_sd(port):
    '''Dumps data from Arduino's SD to one file for further parsing'''
    port.write(b'd')
    blob = open('temp.txt', 'w')
    port = init_serial()
    while (c := port.read().decode('ASCII')) != '|':
        print(c, end='')
        blob.write(c)
    blob.close()

def update_time(port):
    '''Sends data about time and date to the Arduino'''
    port.write(b't')
    port.write(localtime().tm_hour.to_bytes(1,'big'))
    port.write(localtime().tm_min.to_bytes(1, 'big'))
    port.write(localtime().tm_sec.to_bytes(1, 'big'))
    port.write(localtime().tm_mday.to_bytes(1, 'big'))
    port.write(localtime().tm_mon.to_bytes(1, 'big'))
    port.write((localtime().tm_year % 2000).to_bytes(1, 'big'))

def cli():
    '''Command line interface for interacting with Arduino'''
    port = init_serial()
    while (cmd := input('>>> ')) != 'q':
        if cmd == 't':
            update_time(port)
        elif cmd == 'd':
            dump_sd(port)
        else:
            while port.in_waiting:
                print(port.readline())

def recover_file_structure():
    '''Split blob given by the Arduino to seperate files'''
    f = open(BLOB, 'r')
    rows = f.readlines()
    f.close()
    new_file = open('misc.txt', 'w')
    for r in rows:
        if filename := re.match(r'.*\.CSV', r, re.I):
            new_file.close()
            new_file = open(filename.group(), 'w')
        else:
            new_file.writelines(r)


if __name__ == '__main__':
    cli()
