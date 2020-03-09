#!/usr/bin/env python
import rospy
import pyrealsense2 as rs
import numpy as np
import cv2
from cv_bridge import CvBridge
from sensor_msgs.msg import Image

def talker() :
    rospy.init_node('rs_camera_pub', anonymous = False)
    rospy.loginfo('rs_camera_pub node started')

    pipeline = rs.pipeline()
    cobridge = CvBridge() 
    debridge = CvBridge()

    #rs setting start
    config = rs.config()
    config.enable_stream(rs.stream.depth, 640, 480, rs.format.z16, 30)
    config.enable_stream(rs.stream.color, 640, 480, rs.format.bgr8, 30)
    profile = pipeline.start(config)

    depth_sensor = profile.get_device().first_depth_sensor()
    depth_scale = depth_sensor.get_depth_scale()

    #removing background setting
    clipping_distance_in_meters = 2 #1 meter
    clipping_distance = clipping_distance_in_meters / depth_scale

    #aligning image
    align_to = rs.stream.color
    align = rs.align(align_to)

    #publisher 
    colorPub = rospy.Publisher('color_image', Image, queue_size =1)
    depthPub = rospy.Publisher('depth_image', Image, queue_size =1)

    # cv2.namedWindow("img", cv2.WINDOW_NORMAL)
    try:

        while not rospy.is_shutdown():

            frames = pipeline.wait_for_frames()            
            aligned_frames = align.process(frames)
            
            #type : class <pyrealsense2.pyrealsense2.video_frame>
            #aligned_depth_frame is a 640x480 depth image
            aligned_depth_frame = aligned_frames.get_depth_frame() 
            color_frame = aligned_frames.get_color_frame() 

            if not aligned_depth_frame or not color_frame:
                continue

            #type : numpy.ndarray
            color_image = np.asanyarray(color_frame.get_data())
            depth_image = np.asanyarray(aligned_depth_frame.get_data())

            #remove background
            grey_color = 153
            depth_image_3d = np.dstack((depth_image,depth_image,depth_image)) #depth image is 1 channel, color is 3 channels
            bg_removed = np.where((depth_image_3d > clipping_distance) | (depth_image_3d <= 0), grey_color, color_image)

            
            #removed img
            imgMsgCol = cobridge.cv2_to_imgmsg(bg_removed, encoding="bgr8")
            #imgMsgCol = cobridge.cv2_to_imgmsg(color_image, encoding="bgr8")
            imgMsgDep = debridge.cv2_to_imgmsg(depth_image, encoding="16UC1")

            colorPub.publish(imgMsgCol)
            depthPub.publish(imgMsgDep)

            # depth_colormap = cv2.applyColorMap(cv2.convertScaleAbs(depth_image, alpha=0.03), cv2.COLORMAP_JET)
            # cv2.imshow("color_image", color_image)
            # cv2.imshow('depth_colormap', depth_colormap)
            # if cv2.waitKey(1) & 0xFF == ord('q'):
            #     break

            rospy.Rate(30).sleep()
            #^^ talker HZ

    finally :
        cv2.destroyAllWindows()
        pipeline.stop()


if __name__ == "__main__":
    try :
        talker()
    except rospy.ROSInterruptException:
        pass
