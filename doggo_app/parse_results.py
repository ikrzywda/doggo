#!/usr/bin/python

import csv
import string
import random as rnd

USER_NUMBER = 1000

tailing_zero = lambda x : str(x) if x >= 10 else ('0' + str(x))

def dummy_log_filename():
    return tailing_zero(rnd.randint(1,28)) + \
           tailing_zero(rnd.randint(1,12)) + \
           tailing_zero(rnd.randint(1,40)) + '.csv'

def dummy_users():

    user_list = list()

    for i in range(USER_NUMBER):
        user_list.append(['#' + str(i) + ',' +
                          ''.join(rnd.choices(string.digits, k=4)) + ',' + 
                          ''.join(rnd.choices(string.ascii_uppercase, k=12))])

    return user_list


def dummy_runs():
    
    runs = list()
    random_range = rnd.randint(0, 2 * USER_NUMBER)


    for i in range(random_range):
        runs.append(['#' + (str(rnd.randint(0, USER_NUMBER-1))) + ',' + 
                     ''.join(tailing_zero(rnd.randint(0,24)) + ':' 
                         + tailing_zero(rnd.randint(0,60))) + ',' +  
                     str(rnd.randint(0,1))])
    return runs


def dummy_doggo_database(output_dir):
    
    with open((output_dir + '/users.csv'), 'w', newline='\n') as f:
        w = csv.writer(f, delimiter='|')
        w.writerows(dummy_users())
   
    for i in range(100):
        with open((output_dir + '/' + dummy_log_filename()), 
                   'w', newline='\n') as f:
            w = csv.writer(f, delimiter='|')
            w.writerows(dummy_runs())

    
if __name__ == '__main__':
    dummy_doggo_database('tests')
