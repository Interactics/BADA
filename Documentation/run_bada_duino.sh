source ~/catkin_robot/devel/setup.bash
sudo chmod 666 /dev/ttyACM0
rosrun rosserial_python serial_node.py /dev/ttyACM0
