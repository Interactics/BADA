cmake_minimum_required(VERSION 2.8.3)
project(mobile_robot)

add_compile_options(-std=c++11)

find_package(catkin REQUIRED COMPONENTS
  roscpp
  std_msgs
  geometry_msgs
  message_generation
)



 add_message_files(
   FILES
   motor_cmd.msg
 )



 generate_messages(
   DEPENDENCIES
   std_msgs
   geometry_msgs
 )


catkin_package(
  INCLUDE_DIRS include
 
  CATKIN_DEPENDS roscpp std_msgs 
  #DEPENDS system_lib

)


include_directories(include ${catkin_INCLUDE_DIRS} )

 add_executable(motor_node src/motor_algorithm.cpp)
 target_link_libraries(motor_node ${catkin_LIBRARIES} pigpiod_if2)
 add_dependencies(motor_node mobile_robot_generate_messages_cpp)
