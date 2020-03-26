2019-2020 Robot Capstone Project Team : [사과가 쿵]


# Bada
## Overview

Bada is a Social Robot made for Hearing-impaired people.
The word 'BADA' is coming from "Buddy for Auditory Detection Assistance" and South Korean first assistance dog's name.

## Purpose

# Nodes

## Bada_node

### Published Topic

#### ~~bada/cmd_vel(geometry_msgs/Twist)~~
#### bada/real_vel(geometry_msgs/Twist)

**bada/real_vel** is bada's velocity in realworld. 
The linear and angular velocity of bada. Twists.linear.x is the forward velocity [m/s] and Twists.angular.z is the angular velocity [rad/sec]. 

### Subscribed Topic

#### bada/cmd_vel(geometry_msgs/Twist)

**bada/cmd_vel** is a velocity commanded to reach.


### Parameters


# TodoLIST

## Mobile Robot 
 - [x] Encoder value setting
 - [x] Publish Twist message
 - [ ] PID Control
 - [ ] Refactoring code
 
## Sound Localization
 - [x] Measure the direction of sound using TDoA Sensor
 - [ ] Merge TDoA sensor and mobile robot.
 - [ ] Create movement toward where sound is ocurred.
 
## Sound Recognition
 - [x] YamNet 
 - [ ] Make Yamnet as ROS Program.
 - [ ] 
 
## SLAM 
 - [x] Pointcloud using RealSense T265 & D435 
 - [ ] Making SLAM Information

## WebAPP
 - [ ] Interaction Design
 - [ ] Connect WebAPP with ROS 
 - [ ] Visualizing SLAM Information
 
 ## Human-Robot-Interaction
 - [ ] What if the person sleeps.
 - [ ] What if the person is far from their phone.
 - [ ] How to transfer information about danger situation.
