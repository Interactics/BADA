#include <ros/ros.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;

int main(int argc, char** argv){
	ros::init(argc, argv, "simple_navigation_goals");
  	MoveBaseClient actionClient("move_base", true);
	actionClient->waitForServer();
	ROS_INFO("going to sound");
	move_base_msgs::MoveBaseGoal sound_goal;

	sound_goal.target_pose.header.frame_id = "base_link";
	sound_goal.target_pose.header.stamp = ros::Time::now();

	sound_goal.target_pose.pose.position.x = 1;
	sound_goal.target_pose.pose.orientation.z = -0.541;
	sound_goal.target_pose.pose.orientation.w = 0.841;
	actionClient->sendGoal(sound_goal);

	bool finished_before_timeout = actionClient->waitForResult(ros::Duration(30.0));
	ROS_INFO("go to sound done.");

	if (finished_before_timeout)
	{
		actionlib::SimpleClientGoalState state = actionClient->getState();
		ROS_INFO("go to sound Action finished: %s", state.toString().c_str());
	}
	else
		ROS_INFO("go to sound Action did not finish before the time out.");
}
