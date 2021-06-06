#!/usr/bin/python

'''Module for interfacing with doggo Arduino module:
    - refreshing date and time upon connection
    - ability to dump SD file data without taking out the SD'''

from sys import platform
from time import localtime, sleep
import serial

def cli(): 
    port = init_serial()
    UPDATE_TIME = 'update-time'
    DUMP_SD_TO_FILES = 'dump-sd'
    while True:
        port.write(b't')
        port.write(localtime().tm_hour.to_bytes(1,'big'))
        port.write(localtime().tm_min.to_bytes(1,'big'))
        port.write(localtime().tm_sec.to_bytes(1,'big'))
        port.write(localtime().tm_mday.to_bytes(1,'big'))
        port.write(localtime().tm_mon.to_bytes(1,'big'))
        port.write((localtime().tm_year % 2000).to_bytes(1,'big'))
        print(port.readline())
        sleep(1)

def update_time(port):
    port.write(b't')
    port.write(localtime().tm_hour)
    port.write(localtime().tm_min)
    port.write(localtime().tm_sec)
    port.write(localtime().tm_mday)
    port.write(localtime().tm_mon)
    port.write(localtime().tm_year % 2000)

def dump_sd(port):
    pass

def init_serial():
    '''Initializes communication with the module at 115200 baud-rate'''
    port_name = ''
    if platform.startswith('linux'):
        port_name = '/dev/ttyACM0'
    elif platform.startswith('win32'):
        port_name = 'COM7'

    return serial.Serial(port_name, 115200, timeout=None)


def update_time(ser_port):
    '''sends time data to update module's RTC
        (hour, minute, second, day, month, year'''
    ser_port.write(bytes(localtime().tm_hour))
    ser_port.write(bytes(localtime().tm_min))
    ser_port.write(bytes(localtime().tm_sec))
    ser_port.write(bytes(localtime().tm_mday))
    ser_port.write(bytes(localtime().tm_mon))
    ser_port.write(bytes(localtime().tm_year % 2000))





if __name__ == '__main__':
    cli()
