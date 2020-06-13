2019-2020 Robot Capstone Project 
Team : [사과가 쿵!]

Member : 유호연, 김민우, 배종학, 이현우, 최수진, 황지원

Directed by Prof. 한재권

# BADA : The social Robot for Hearing-impaired people.

### 2020 CAPSTONE DESIGN FAIR Grand prize @ Hanyang University ERICA Engineering College. 
## Overview

Bada is a Social Robot made for Hearing-impaired people.
The word 'BADA' derive from "Buddy for Auditory Detecting Assistance" and South Korean first assistance dog's name.

## Purpose


## Requirement

### Raspberry pi 3B+

#### install pigpiod

    wget https://github.com/joan2937/pigpio/archive/master.zip
    unzip master.zip
    cd pigpio-master 
    make
    sudo make install


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

## signal_node

### Published Topic

#### /signal

Detected sound signal. type: std_msgs/String

#### /audio

Realtime sound detection informations.
JSON array contains detected sound data with [name, probability] format.

type: std_msgs/String 
format: `{'data': <JSON array>}`

##### usage

```js
var detectionData = JSON.parse(msg.data);
```

# TodoLIST
### Final
 - [ ] refactoring
 
## Robot System Core
 - [x] Architecture Design
 - [x] Coding 
 - [x] Debug 

## Mobile Robot 
 - [x] Encoder value setting
 - [x] Publish Twist message
 - [x] PID Control
 - [x] Code Refactoring 
 - [x] Dead Reckoning Information (Odometry / Pose)
 - [x] Code Refactoring
 - [x] TF message Generation
 - [x] Trajectory 
 - [x] Navigation
 - [x] TF Change
 - [x] Making roslaunch
 
## Sound Localization
 - [x] Measure the direction of sound using TDoA Sensor
 - [x] Merge TDoA sensor and mobile robot.
 - [x] Create movement toward where sound is ocurred.
 
## [Sound Recognition](Sound/README.md)
 - [x] YamNet 
 - [x] Make Yamnet as ROS Program.
 
## SLAM 
 - [x] Pointcloud using RealSense T265 & D435 
 - [x] Making SLAM Information
 - [x] GMAPPing 
 - [x] Navigation
 
## [WebAPP](Visualization/README.md)

 - [x] Interaction Design
 - [x] Connect WebAPP with ROS 
 - [x] Visualizing SLAM Information
 
 ## Human-Robot-Interaction
 - [x] What if the person sleeps.
 - [x] What if the person is far from their phone.
 - [x] How to transfer information about danger situation.
