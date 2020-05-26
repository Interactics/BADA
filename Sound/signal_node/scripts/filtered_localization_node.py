#!/usr/bin/env python
import sys
print('python version: ',sys.version_info)

import pyaudio
import numpy as np
from matplotlib import pyplot as plt

import soundfile as sf

import params
import yamnet as yamnet_model

import time
import queue

import roslibpy
import rospy
import json

import os

from std_msgs.msg import String
from geometry_msgs.msg import PoseStamped

from audio_common_msgs.msg import AudioData

def callback(msg):
    print(msg)
    

def filtered_localization_node():
    # global pub, keys, signals, picked, frames
    rospy.init_node('filtered_localization_node', anonymous=True)
    rospy.loginfo('starting')

    rospy.Subscriber("/sound_localization", PoseStamped, callback)
    rospy.spin()

if __name__ == '__main__':
    filtered_localization_node()

    # except rospy.ROSInterruptException:
    #     pass