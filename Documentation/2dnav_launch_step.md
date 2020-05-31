## 카메라 키기

```bash
sb # source ~/catkin_ws/devel/setup.bash

roslaunch realsense2_camera rs_d400_and_t265.launch 
```

## rtabmap localization mode 

```bash
cr # source ~/catkin_robot/devel/setup.bash

roslaunch bada_2dnav bada_rtabmap.launch localization:=true

#    [ INFO] [1590866980.029912881]: rtabmap (345): Rate=0.50s, Limit=0.000s, RTAB-Map=0.2056s, Maps update=0.0035s pub=0.0000s (local map=967, WM=967)
#    이런 형식 나올때까지 기다리기

```