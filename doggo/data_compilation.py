#!/bin/python

import csv
import sys

def timestr_to_minutes(timestr):
    split_date = timestr.split(':')
    time_in_minutes = int(split_date[0]) * 60 + int(split_date[1])
    return time_in_minutes

def sort_data(source_file):
    sorted_data = list()
    usr_ids = dict()
    with open(source_file) as f:
        reader = csv.reader(f, delimiter=',')
        for row in reader:
            sorted_data.append((row[0], timestr_to_minutes(row[1])))
            usr_ids[row[0]] = 0
    sorted_data = sorted(sorted_data, key=lambda tup : tup[0])
    return (sorted_data, usr_ids)

def compute_walk_times(sorted_data):
    # TODO: sum up time and assign it to according ID
    records = sorted_data[0]
    user_dict = sorted_data[1]
    current_user = records[0]
    for i in range(1, len(records)):
        print(current_user)
        if current_user is None:
            current_user = records[i]
            print(records[i])
        elif current_user[0] == records[i][0]:
            user_dict[current_user[0]] += records[i][1] - current_user[1]
            current_user = None
    return user_dict

if __name__ == '__main__':
    data_packet = sort_data(sys.argv[1])
    print(compute_walk_times((data_packet[0], data_packet[1])))

