#!/usr/bin/env python
import sys
print('python version: ',sys.version_info)

import pyaudio
import numpy as np
from matplotlib import pyplot as plt

import cmath
import math

import soundfile as sf

import params
import yamnet as yamnet_model

import time
import queue

import roslibpy
import rospy
import json

import os

import tf

from std_msgs.msg import String
from std_msgs.msg import Int32
from geometry_msgs.msg import PoseStamped, Quaternion
from audio_common_msgs.msg import AudioData
from tf.transformations import quaternion_from_euler

reset_seconds=2 # max time difference between two signal
last_second=0 # when the last message received
reset=False # whether to reset queue state

sq=queue.Queue()
qsize=0
cumsum=0j
mean=0
pub=''

# https://en.wikipedia.org/wiki/Mean_of_circular_quantities

def callback(msg):
    global sq, qsize, last_second, reset, pub, mean
    # print(msg)

    # (r, p, y) = tf.transformations.euler_from_quaternion([msg.pose.orientation.x, msg.pose.orientation.y, msg.pose.orientation.z, msg.pose.orientation.w])

    current_second=msg.header.stamp.secs

    if(current_second - last_second > reset_seconds):
        reset=True
        print('RESET QUEUE')
    last_second=current_second
    quat=quaternion_from_euler(0,0,mean)
    msg.pose.orientation=Quaternion(quat[0],quat[1],quat[2],quat[3])
    pub.publish(msg)

    print('-----------callback----------')
    
def callback_direction(msg):
    global sq, qsize, cumsum, mean, reset, pub
    if(reset):
        reset=False
        qsize=0
        sq=queue.Queue()
        cumsum=0
        
    print(msg)
    val=-msg.data+180
    if(val>360):
        val-=360
    val=val*math.pi/180
    sq.put(cmath.exp(1j*val))
    cumsum+=cmath.exp(1j*val)
    qsize+=1
    
    if(qsize<3):
        mean=cmath.phase(cumsum)
    else:
        qsize-=1
        cumsum-=sq.get()
        mean=cmath.phase(cumsum)
    print('mean', mean)
    print('----------callback_direction------------')
    

def filtered_localization_node():
    global pub
    # global pub, keys, signals, picked, frames
    rospy.init_node('filtered_localization_node', anonymous=True)
    rospy.loginfo('starting')

    rospy.Subscriber("/sound_localization", PoseStamped, callback)
    rospy.Subscriber("/sound_direction", Int32, callback_direction)
    pub = rospy.Publisher('/sound_localization_filtered', PoseStamped)
    
    rospy.spin()

if __name__ == '__main__':
    filtered_localization_node()

    # except rospy.ROSInterruptException:
    #     pass