import csv
import pathlib
import os
import fileinput
import sys
import operator

hour_in = list()
hour_out = list()
namesloc = list()
names = list()
in_numbers = list()
out_numbers = list()
board_to_show = dict()

workdir = pathlib.Path('Desktop\\Programming\\csvfilesdoggo\\')
files_in_workdir = workdir.iterdir()
for item in files_in_workdir:
    namesloc.append('Desktop\\Programming\\csvfilesdoggo\\' + item.name)
    x = item.name.replace('.csv' , '')
    x = x + ' spędził/ła: '
    names.append(x)

i = len(namesloc)
while i > 0:
    with open(namesloc[i-1] , 'r') as f:
        reader = csv.reader(f , delimiter = ',')
        for row in reader:
            hour_in.append(row[0])
            hour_out.append(row[1])
    i-=1

def parse():
    i = 1
    while i < len(hour_in):
        in_numbers.append(int(hour_in[i]))
        out_numbers.append(int(hour_out[i]))
        i += 2

def value_calculate():
    i = 0
    x = len(names)-1
    godz = ' godzin '
    mins = ' minut na spacerach z psami!'
    while i < len(in_numbers):
        rank = out_numbers[i] - in_numbers[i]
        hours = rank
        minutes = rank
        rank = float(rank/3600000)
        hours = int(hours/3600000)
        minutes = minutes%3600000
        minutes = int(minutes/60000)
        if hours == 1:
            godz = ' godzinę '
        if hours == 2 or hours == 3 or hours == 4:
            godz = ' godziny '
        if mins == 1:
            mins = ' minutę na spacerach z psami!'
        if mins == 2 or mins == 3 or mins == 4:
            mins = ' minuty na spacerach z psami!'
        board_to_show.update({names[x] : str(hours)+ godz +str(minutes)+ mins})
        i +=1
        x -= 1
        
def make_leaderboard_to_show():
    sorted_board = sorted(board_to_show.items(), key=operator.itemgetter(1), reverse=True)
    return sorted_board

parse()
value_calculate()
make_leaderboard_to_show()
