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
board = dict()

workdir = pathlib.Path('Desktop\\Programming\\csvfilesdoggo\\')
files_in_workdir = workdir.iterdir()
for item in files_in_workdir:
    namesloc.append('Desktop\\Programming\\csvfilesdoggo\\' + item.name)
    x = item.name.replace('.csv' , '')
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
    while i < len(in_numbers):
       board.update({names[x] : out_numbers[i] - in_numbers[i]})
       i +=1
       x -= 1

def make_leaderboard():
    sorted_board = sorted(board.items(), key=operator.itemgetter(1), reverse=True)
    print(sorted_board)
    

parse()
value_calculate()
make_leaderboard()
