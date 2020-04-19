sudo apt-get install -y ros-melodic-catkin;

sudo apt-get install -y cmake python-catkin-pkg python-empy python-nose python-setuptools libgtest-dev build-essential;

mkdir build && cd build && cmake ../ && make && sudo make install;

mkdir -p ~/catkin_ws/src;
cd ~/catkin_ws/;
catkin_make;

# python 3:  
# catkin_make -DPYTHON_EXECUTABLE=/usr/bin/python3

source devel/setup.bash;

