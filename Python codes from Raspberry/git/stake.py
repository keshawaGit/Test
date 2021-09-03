from datetime import datetime
#import time
from hashlib import sha256
import json, requests
import random
from time import time
from timer import Timer
import serial

t = Timer()

class Node:
    def __init__(self, id, ageValue, stake):
        self.id = id
        self.ageValue = ageValue
        self.stake = stake

def pickwinner(list):

    weight = []

    for i in list:
        weight.append(int(i.ageValue * i.stake))

    print(weight)

    n = len(weight)
    max_weight = largest(weight,n)
    winner =  weight.index(max_weight)+1

    # for j in weight:
    #     if winner == weigth[j]:
    #         node_id =
    print ("Winner is Node",winner)

    setAgeValue(weight, winner)

def largest(arr,n):
    # Initialize maximum element
    max = arr[0]

    # Traverse array elements from second
    # and compare every element with
    # current max
    for i in range(1, n):
        if arr[i] > max:
            max = arr[i]
    return max

def setAgeValue(weight_list, winner_node):
    for i in nodelist:
        if i.id == winner_node: i.ageValue=0
        else: i.ageValue += 1

    for j in nodelist:
        print(j.ageValue)

ser = serial.Serial('/dev/ttyUSB1', 115200, timeout=1)
ser.flush()
ser.write(str(11).encode('utf-8'))
t.start()

nodelist = []

nodelist.append(Node(1,1,30))
nodelist.append(Node(2,1,40))
nodelist.append(Node(3,1,47))
nodelist.append(Node(4,2,60))
nodelist.append(Node(5,0,97))
nodelist.append(Node(6,2,54))
nodelist.append(Node(7,1,63))
nodelist.append(Node(8,3,26))
nodelist.append(Node(9,1,70))
nodelist.append(Node(10,1,84))

for j in nodelist:
    print(j.ageValue)

for x in range(6):
    pickwinner(nodelist)
t.stop()
ser.write(str(0).encode('utf-8'))