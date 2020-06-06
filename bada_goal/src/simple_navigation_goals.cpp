#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>

typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
  ros::init(argc, argv, "simple_navigation_goals");

  //tell the action client that we want to spin a thread by default
  MoveBaseClient actionClient("move_base", true);

  //wait for the action server to come up
  actionClient.waitForServer();
	ROS_INFO("going to sound");
	move_base_msgs::MoveBaseGoal sound_goal;
	move_base_msgs::MoveBaseGoal sound_goal2;

	sound_goal.target_pose.header.frame_id = "base_link";
	sound_goal.target_pose.header.stamp = ros::Time::now();

	sound_goal.target_pose.pose.position.x = 0;
	sound_goal.target_pose.pose.orientation.z = -0.621;
	sound_goal.target_pose.pose.orientation.w = 0.784;
	actionClient.sendGoal(sound_goal);
	actionClient.waitForResult(ros::Duration(30.0));

	sound_goal2.target_pose.header.frame_id = "base_link";
	sound_goal2.target_pose.header.stamp = ros::Time::now();

	sound_goal2.target_pose.pose.position.x = 1.0;
	sound_goal2.target_pose.pose.orientation.w = 1.0;
	actionClient.sendGoal(sound_goal2);
  if(actionClient.getState() == actionlib::SimpleClientGoalState::SUCCEEDED)
    ROS_INFO("Hooray, the base moved 1 meter forward");
  else
    ROS_INFO("The base failed to move forward 1 meter for some reason");

  return 0;
}
