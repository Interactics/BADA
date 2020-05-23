#!/usr/bin/env python

from cv_bridge import CvBridge, CvBridgeError
from sensor_msgs.msg import Image
from std_msgs.msg import Float32
import roslib
import rospy
import numpy as np
from jsk_recognition_msgs.msg import RectArray

global xmin, ymin, xmax, ymax


##/camera/aligned_depth_to_color/image_raw
global depth_colormap

def DepImgSUB(img_msg) : 
    img = msg_to_numpy(img_msg)
    depthimg_np = np.asarray(img)

        ## TESTCODE

    depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(img_np, alpha = 0.03), cv2.COLORMAP_JET)      ## type : numpy.ndarray


    cv2.imshow('rs', depth_colormap)
    cv2.waitKey(1)

def WhereIsHuman(bbox);
    global depth_colormap
    global xmin, ymin, xmax, ymax

    xmin = bbox.rects[0].x
    ymin = bbox.rects[0].y
    xmax = xmin +  bbox.rects[0].width
    ymax = ymin +  bbox.rects[0].height
    pt_bbox =  (xmin, ymin, xmax, ymax)
    ROI_BBOX =  depth_colormap[xmin:xmax, ymin:ymax]
    distance = np.median(ROI_BBOX)
    print 'distantce : ', distance

def Listener() :
    rospy.init_node('bada_eyes', anonymous = False)
    rospy.Subscriber("/camera/aligned_depth_to_color/image_raw", Image, DepImgSUB)
    rospy.Subscriber("/edgetpu_object_detector/output/rects", RectArray, WhereIsHuman)

    rospy.loginfo('bada_eyes')

    imgMsgDep = debridge.cv2_to_imgmsg(depth_image, encoding="16UC1")

def msg_to_numpy(data):
        bridge = CvBridge()
        try:
            raw_img = bridge.imgmsg_to_cv2(data, "16UC1")
        except CvBridgeError as err:
            print("err")