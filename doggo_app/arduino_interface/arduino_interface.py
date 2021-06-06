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


def update_time(ser_port):
    '''sends time data to update module's RTC
        (hour, minute, second, day, month, year'''
    ser_port.write(bytes(localtime().tm_hour))
    ser_port.write(bytes(localtime().tm_min))
    ser_port.write(bytes(localtime().tm_sec))
    ser_port.write(bytes(localtime().tm_mday))
    ser_port.write(bytes(localtime().tm_mon))
    ser_port.write(bytes(localtime().tm_year % 2000))



def run_connection():
    '''main function for interfacing with the system'''
    port = init_serial()
    port.write(b'd')
    while True:
        print(port.readline())
        sleep(1)

if __name__ == '__main__':
    run_connection()
