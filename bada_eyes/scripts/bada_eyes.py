#!/usr/bin/env python
from sensor_msgs.msg import Image
from std_msgs.msg import Int32
from std_msgs.msg import Bool
from geometry_msgs.msg import Point
from jsk_recognition_msgs.msg import RectArray
from cv_bridge import CvBridge, CvBridgeError

import roslib
import rospy
import cv2
import math
import numpy as np

global depthimg_np, depth_colormap, eyes_status

eyes_status = False
start_node = False
eyes_open  = False

height = 480
width = 640
DistTheta = Point()

rospy.init_node('bada_eyes', anonymous = False)
rate = rospy.Rate(5) # ROS Rate at 5Hz

pubDist   = rospy.Publisher('bada/eyes/distance', Point, queue_size=1)

def SubEyesOpen(data):
    global eyes_open, eyes_status
    eyes_open = data.data

def DepImgSUB(img_msg) : 
    global depthimg_np, depth_colormap, start_node

    if eyes_open == False :   return 
    img = msg_to_numpy(img_msg)
    depthimg_np = np.asarray(img)
    start_node = True

def WhereIsHuman(bbox) :
    global DistTheta
    global depthimg_np
    global depth_colormap

    if eyes_open == False :   return 
    if start_node == False :  return 
    if len(bbox.rects) == 0 : return 

    xmin = bbox.rects[0].x
    ymin = bbox.rects[0].y
    xmax = xmin + bbox.rects[0].width
    ymax = ymin + bbox.rects[0].height
    x_mid = (xmin+xmax)/2
    y_mid = (ymin+ymax)/2
    
    ROI_BBOX = depthimg_np[ymin:ymax, xmin:xmax]
    distance = np.median(ROI_BBOX)

    dist_x = float(getHorizontalCoordinate(x_mid, distance))
    dist_y = float(getVerticalCoordinate(y_mid, distance))

    Rad = math.atan2(dist_x, distance)

    DistTheta.x =  distance # Distance
    DistTheta.y =  Rad      # Radian
    DistTheta.z =  0        # NULL

    pubDist.publish(DistTheta)
    rate.sleep()

def msg_to_numpy(data):
        bridge = CvBridge()
        try:
            raw_img = bridge.imgmsg_to_cv2(data, "16UC1")
        except CvBridgeError as err:
            print("err")

        return raw_img


def getVerticalCoordinate(y, distance):
    #rs RGB : FOV 69.4 x 42.5 x 77 (HxVxD)
    #rs Depth : FOV 73 x 58 x 95 (HxVxD)

    VFov2 = math.radians(42.5/2)
    VSize = math.tan(VFov2) * 2
    VCenter = (height-1)/2
    VPixel = VSize/(height - 1)
    VRatio = (VCenter - y) * VPixel

    return distance * VRatio

def getHorizontalCoordinate(x, distance):
    #rs RGB : FOV 69.4 x 42.5 x 77 (HxVxD)
    #rs Depth : FOV 73 x 58 x 95 (HxVxD)

    HFov2 = math.radians(64.4/2)
    HSize = math.tan(HFov2) * 2
    HCenter = (width-1)/2
    HPixel = HSize/(width - 1)
    HRatio = (x - HCenter) * HPixel
    return distance * HRatio

def bada_eyes_func() :
    rospy.Subscriber("/camera/aligned_depth_to_color/image_raw", Image, DepImgSUB)
    rospy.Subscriber("/edgetpu_object_detector/output/rects", RectArray, WhereIsHuman)
    rospy.Subscriber('/bada/eyes/open', Bool, SubEyesOpen)

    rospy.loginfo('bada_eyes is up')
    rospy.spin()

if __name__ == '__main__' :
    try:   
        bada_eyes_func()
    except rospy.ROSInterruptException:
        pass