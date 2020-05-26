#!/usr/bin/env python
import sys
print('python version: ',sys.version_info)

import pyaudio
import numpy as np
from matplotlib import pyplot as plt

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

# so the data is signal based, so i have to consider whether two signal is related or not.
# then i can use timestamp data for that purpose.
# the problem is, the mean filter will be reset quite often, according to the threshold duration.
# 
reset_seconds=2 # max time difference between two signal
last_second=0 # when the last message received
reset=False # whether to reset queue state

sq=queue.Queue()
qsize=0
sinesum=0
cumsum=0
mean=0
pub=''

# https://en.wikipedia.org/wiki/Mean_of_circular_quantities

def callback(msg):
    global sq, qsize, last_second, reset, pub, mean
    # print(msg)
    # (r, p, y) = tf.transformations.euler_from_quaternion([msg.pose.orientation.x, msg.pose.orientation.y, msg.pose.orientation.z, msg.pose.orientation.w])
    # print(r,p,y)
    # print(y*180/(math.pi))
    current_second=msg.header.stamp.secs
    # print(current_second - last_second)
    if(current_second - last_second > reset_seconds):
        reset=True
        print('RESET QUEUE')
    last_second=current_second
    # newmsg=PoseStamped(msg)
    # print(type(msg.pose.orientation))
    quat=quaternion_from_euler(0,0,mean*math.pi/180.0)
    msg.pose.orientation=Quaternion(quat[0],quat[1],quat[2],quat[3])
    # print(type(msg.pose.orientation))
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

    sq.put(val)
    cumsum+=val
    qsize+=1
    
    if(qsize<3):
        mean=cumsum/qsize
    else:
        qsize-=1
        cumsum-=sq.get()
        mean=cumsum/qsize
    print('mean', mean)
        
    # sq.put(msg)
    # qsize+=1
    # if(qsize>5):
    #     qsize-=1
    #     now=
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