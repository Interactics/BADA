2019-2020 Robot Capstone Project Team : [사과가 쿵]


# Bada
## Overview

Bada is a Social Robot made for Hearing-impaired people.
The word 'BADA' is coming from "Buddy for Auditory Detection Assistance" and South Korean first assistance dog's name.

## Purpose

# Nodes

## Bada_node

### Published Topic

#### /bada/odom(nav_msgs/Odometry)

Odometry Information of Bada. `Pose` and `velocity` are the Position and Velocity of Bada in Global reference frame.

#### /TF

TF Information of Bada.

### Subscribed Topic
#### /cmd_vel(geometry_msgs/Twist)

The linear and angular velocity of Bada in Bada's Local reference fream. `Twists.linear.x` is the forward velocity [m/s], `Twists.angular.z` is the angular velocity [rad/s].

### Parameters


# TodoLIST

## Mobile Robot 
 - [x] Encoder value setting
 - [x] Publish Twist message
 - [x] PID Control
 - [x] Code Refactoring 
 - [x] Dead Reckoning Information (Odometry / Pose)
 - [x] Code Refactoring
 - [x] TF message Generation
 - [ ] Trajectory 
 - [ ] Navigation
 - [ ] Making roslaunch
 
## Sound Localization
 - [x] Measure the direction of sound using TDoA Sensor
 - [ ] Merge TDoA sensor and mobile robot.
 - [ ] Create movement toward where sound is ocurred.
 
## Sound Recognition
 - [x] YamNet 
 - [ ] Make Yamnet as ROS Program.
 
## SLAM 
 - [x] Pointcloud using RealSense T265 & D435 
 - [x] Making SLAM Information
 - [x] GMAPPing 
 - [ ] Navigation
 
## [WebAPP](Visualization/README.md)

 - [ ] Interaction Design
 - [ ] Connect WebAPP with ROS 
 - [ ] Visualizing SLAM Information
 
 ## Human-Robot-Interaction
 - [ ] What if the person sleeps.
 - [ ] What if the person is far from their phone.
 - [ ] How to transfer information about danger situation.
