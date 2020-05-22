#!/usr/bin/env python

from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image
from std_msgs.msg import Int32
import roslib
import rospy
import numpy as np
import cv2

from jsk_recognition_msgs.msg import RectArray

global depthimg_np
global depth_colormap
global start_node 
start_node = False

pubDist = rospy.Publisher('bada/eyes/distance', Int32, queue_size =1)

def DepImgSUB(img_msg) : 
    global depthimg_np
    global depth_colormap
    global start_node 

    img = msg_to_numpy(img_msg)
    depthimg_np = np.asarray(img)
    start_node = True
    #depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(depthimg_np, alpha = 0.03), cv2.COLORMAP_JET)      ## type : numpy.ndarray

    # cv2.imshow('rs', depth_colormap)
    # cv2.waitKey(1)

def WhereIsHuman(bbox) :
    # global depthimg_np
    # global depth_colormap

    if start_node == False : 
        return 
    if len(bbox.rects) == 0 :
        return 
    xmin = bbox.rects[0].x
    ymin = bbox.rects[0].y
    xmax = xmin +  bbox.rects[0].width
    ymax = ymin +  bbox.rects[0].height
    ROI_BBOX =  depthimg_np[ymin:ymax, xmin:xmax]
    distance = np.median(ROI_BBOX)

    # print ROI_BBOX.shape
    # print 'distantce : ', distance
    # cv2.rectangle(depth_colormap, (xmin, ymin), (xmax, ymax), (0,255,0), 3)
    # ROI_BBOX = cv2.applyColorMap(cv2.convertScaleAbs(ROI_BBOX, alpha = 0.03), cv2.COLORMAP_JET)
   
    pubDist.publish(distance)

    # cv2.imshow('rs1', ROI_BBOX)
    # cv2.waitKey(1)


def bada_eyes_func() :
    rospy.init_node('bada_eyes', anonymous = False)
    rospy.Subscriber("/camera/aligned_depth_to_color/image_raw", Image, DepImgSUB)
    rospy.Subscriber("/edgetpu_object_detector/output/rects", RectArray, WhereIsHuman)
    
    rospy.loginfo('bada_eyes is On')
    rospy.spin()

def msg_to_numpy(data):
        bridge = CvBridge()
        try:
            raw_img = bridge.imgmsg_to_cv2(data, "16UC1")
        except CvBridgeError as err:
            print("err")

        return raw_img

if __name__ == '__main__' :
    bada_eyes_func()