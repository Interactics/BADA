#!/usr/bin/env python

import roslib
import rospy
import tf
from nav_msgs.msg import Odometry
import time

def callback(rs_odom):
	global lx, ly, lz, ax, ay, az
	lx = rs_odom.twist.twist.linear.x
	ly = rs_odom.twist.twist.linear.y
	lz = rs_odom.twist.twist.linear.z

	ax = rs_odom.twist.twist.angular.x
	ay = rs_odom.twist.twist.angular.y
	az = rs_odom.twist.twist.angular.z

if __name__ == "__main__":
	rospy.init_node("make_new_odom", anonymous= True)
	listener = tf.TransformListener()
	sub_odom = rospy.Subscriber("t265/odom/sample",Odometry, callback)
	pose_pub = rospy.Publisher("odom_baselink",Odometry, queue_size=10)
	pose_msg = Odometry()
	br = tf.TransformBroadcaster()
	pose_msg.pose.covariance =[0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001]
	pose_msg.twist.covariance=[0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.1, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.001]
	
	pose_msg.header.frame_id = "odom"
	pose_msg.child_frame_id = "base_link"	
	rate=rospy.Rate(50)

	
	while not rospy.is_shutdown():
		try:
			(trans,rot)= listener.lookupTransform('/t265_odom_frame','/t265_pose_frame',rospy.Time(0))
			trans[0]-=0.088
			trans[2]+=0.0
			pose_msg.pose.pose.position.x = trans[0]
			pose_msg.pose.pose.position.y = trans[1]
			pose_msg.pose.pose.position.z = trans[2]
			pose_msg.pose.pose.orientation.x = rot[0]
			pose_msg.pose.pose.orientation.y = rot[1]
			pose_msg.pose.pose.orientation.z = rot[2]
			pose_msg.pose.pose.orientation.w = rot[3]
			
			pose_msg.twist.twist.linear.x = lx
			pose_msg.twist.twist.linear.y = ly
			pose_msg.twist.twist.linear.z = lz

			pose_msg.twist.twist.angular.x = ax
			pose_msg.twist.twist.angular.y = ay
			pose_msg.twist.twist.angular.z = az
			
			now = rospy.get_rostime()
			pose_msg.header.stamp.secs = now.secs
			pose_msg.header.stamp.nsecs = now.nsecs

			pose_pub.publish(pose_msg)
			br.sendTransform((trans[0],trans[1],trans[2]),(rot[0],rot[1],rot[2],rot[3]),rospy.Time.now(),"base_link","odom")
			rate.sleep()

		except (tf.LookupException, tf.ConnectivityException, tf.ExtrapolationException):
			continue
