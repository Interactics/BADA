2019-2020 Robot Capstone Project Team : [사과가 쿵]


# 1. Bada
## 1.1 Overview

>Bada is a Social Robot made for Hearing-impaired people.
The word 'BADA' is coming from "Buddy for Auditory Detection Assistance" and South Korean first assistance dog's name.

## 1.2 Purpose

# 2. Nodes

## 2.1 Bada_node

### 2.1.1 Published Topic

bada/cmd_vel(geometry_msgs/Twist)

> The linear and angular velocity of bada. Twists.linear.x is the forward velocity [m/s] and Twists.angular.z is the angular velocity [rad/sec]. 

### 2.1.2 Subscribed Topic

### 2.1.3 Parameters


# 0. TodoLIST

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
 
## SLAM 
