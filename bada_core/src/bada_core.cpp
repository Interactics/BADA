#include <bada_ctrl/bada_ctrl.h>
#include <ros/ros.h>
#include <pigpiod_if2.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Empty.h>


std_msgs::Bool HEAD_STATUS;
std_msgs::Int16 MAT_STATUS;

ros::NodeHandle nh;
ros::Publisher bada_camera   = nh.advertise<std_msgs::Bool> ("/bada/duino/camera_cmd", 1);
ros::Publisher bada_display  = nh.advertise<std_msgs::Int16>("/bada/duino/display_cmd", 1);
ros::Subscriber pepl_checker = nh.subscribe<std_msgs::Int32>("/bada/eyes/distance"); //만약 사람이 잡히면 거리가 뜸
ros::Subscriber sig_checker  = nh.subscribe<std_msgs::Empty>("/bada/signal/checker", 1, sig_income); //만약 사람이 잡히면 거리가 뜸
bool sig_checker = false;} // Roaming 단계에서 사용. 


void sig_income(const std_msgs::Empty &msg){
	sig_checker = true;
}// Roaming 단계에서 사용. 소리가 발생할 경우에 쓸모가 있다. 

bool bada_head_CTRL(){
	bada_camera.publish(HEAD_STATUS);
}

enum STATE{
	FINDING_PEPL,
	ROAMING,
	SOUND_DETECTING,
	MOVING_TO_PEPL,
	MOVING_WITH_PEPL
};

int main(int argc, char **argv){
	STATE state = FINDING_PEPL;
	ros::init(argc, argv, "/bada/core");
    ros::Rate loop_rate(6);

	while (ros::ok()){
		switch (state){
		case FINDING_PEPL:
			bool is_there_pepl = false;
			do{
				bada_go_destination();
				// Move to POINT
				// Go to Destination then STOP
				// HEAD_UP
				bada_head_CTRL(true);
				is_there_pepl = bada_rounding();
				bada_head_CTRL(false);
			} while (!is_there_pepl);
			bada_next_state(state);
			break;
		case ROAMING:
			bada_go_roaming();
			bada_next_state(state);
			break;
		case SOUND_DETECTING:
			// 소리 난 방향 서브스크라이빙하기
			// 소리 발생한 방향으로 이동하기 
			// 저장하기 
			bada_next_state(state);
			break;
		case MOVING_TO_PEPL:
		
			bada_next_state(state);
			break;
		case MOVING_WITH_PEPL:
			break;
		} // END_SWITCH
		ros::spinOnce();
	}	  // END_WHILE
} // END_MAIN


void bada_next_state(STATE& S){
	if (S != MOVING_WITH_PEPL)
		S++;
	else if(S == MOVING_WITH_PEPL);
	// DONE;
}

void bada_go_roaming(){
	// GOAL 지정 

	while(){
		ros::spinOnce();
		if (sig_checker) break;
		if () {
			//만약 끝나면 골 위치에 도달하면...
		    // 다음 위치로 이동  
		}
	}
	// navi 해제.

}