# Bada Launch Step

```sh
# run bada_eyes
source ~/coral_ws/devel/setup.bash
roslaunch coral_usb edgetpu_object_detector.launch

source ~/catkin_robot/devel/setup.bash
rosrun bada_eyes bada_eyes.py
##------------------------------------

# run bada_duino 
source ~/catkin_robot/devel/setup.bash
sudo chmod 666 /dev/ttyACM0
rosrun rosserial_python serial_node.py /dev/ttyACM0
##------------------------------------


# run bada_base -- same password
# conpi
# sudo pigpid 
# rosrun bada_base bada_motorCtrl

## run respeaker
source ~/dev/venv/bin/activate # activate python3 virtualenv
roslaunch bada_audio bada_audio.launch
# rosrun bada_audio bada_audio.py


## run camera
source ~/catkin_ws/devel/setup.bash # sb

roslaunch realsense2_camera rs_d400_and_t265.launch 

source ~/catkin_robot/devel/setup.bash # cr

roslaunch bada_2dnav bada_rtabmap.launch localization:=true

#    [ INFO] [1590866980.029912881]: rtabmap (345): Rate=0.50s, Limit=0.000s, RTAB-Map=0.2056s, Maps update=0.0035s pub=0.0000s (local map=967, WM=967)
#    이런 형식 나올때까지 기다리기

```