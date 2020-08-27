# BADA : The social Robot for Hearing-impaired people.

 2019-2020 Robot Capstone Project 
## 2020 CAPSTONE DESIGN FAIR Grand prize @ Hanyang University ERICA. 

### Team : [사과가 쿵!]

Member : 유호연, 김민우, 배종학, 이현우, 최수진, 황지원

Directed by Prof. 한재권


## Overview

BADA is a Social Robot made for Hearing-impaired people.
The word 'BADA' is derived from "Buddy for Auditory Detection Assistance" and South Korean first assistance dog's name.

This project is inspired by hearing dog which can assists hearing-impaired people by acting from the situation of things and modeled on their behaviors.
If BADA detects dangerous sounds such as Fire alarm, this robot firstly send sound information to BADA Web page and KAKAO Talk message for people in thier home and displays pictogram of the sounds source and find person who live in. However, to prepare in situations that people cannot interact with those interfaces, BADA will bump into him/her to delivery sound information. 

BADA can hear 6 sounds source that is selected on the basis of Korean hearing-dog education guide. these are Fire Alarm, Door Knocking, Water Boiling, Water, Baby crying and Door Bell. By utilizing YAMNet which is a machine learning algorithm of sound recognition, this detection system can be constructed. 

RTAB Map, one of the state of the arts 3D SLAM package, helps BADA roam around house.

## Hardware Architecture

Raspberry pi 3B+, NUC, ReSpeaker Microphone Array, Coral USB accelerator, Intel Realsense D435, T265, Adafruit Dot LED Matrix, Arduino mega

![SYSTEM 아키텍쳐](https://user-images.githubusercontent.com/56077549/91379019-ad477c80-e85c-11ea-8cf9-dd4ddd90c1bc.png)

## Action Flowchart

![action알고리즘](https://user-images.githubusercontent.com/56077549/91379042-bb959880-e85c-11ea-93c5-d4fcb14c21d6.png)

## Interface 

### Kakao Talk

![kaka](https://user-images.githubusercontent.com/56077549/91381735-0e724e80-e863-11ea-8d5b-6a415960aca6.png)



### Web

![그림1](https://user-images.githubusercontent.com/56077549/91381727-0adec780-e863-11ea-9a8c-85d8f375ef24.png)


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
