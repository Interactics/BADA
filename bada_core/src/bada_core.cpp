#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <geometry_msgs/Pose2D.h>
#include <geometry_msgs/Point.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Bool.h>
#include <std_msgs/Empty.h>
#include <tf2_geometry_msgs/tf2_geometry_msgs.h>
#include <move_base_msgs/MoveBaseAction.h>
#include <actionlib/client/simple_action_client.h>
#include <tf/transform_listener.h>
#include <math.h>


//---------------- 노드 변경할 것 ----------------------------------------
	/*
	TODO
	3. 각 상태마다 디스플레이에 이미지 표현하도록. 변경
	*/ 
//================================================================================

enum STATE{
	FINDING_PEPL,
	ROAMING,
	SOUND_DETECTING,
	MOVING_TO_PEPL,
	MOVING_WITH_PEPL
};

enum DISP_EVNT {
  NOTHING = 0,
  A_UP, A_DOWN, A_LEFT, A_RIGHT,
  FIRE_EVENT, WATER_EVENT, DOOR_EVENT, BELL_EVENT, BOILING_EVENT, CRYING_EVENT
};

typedef std_msgs::Bool BoolMsg;

std_msgs::Bool HEAD_STATUS;
std_msgs::Int16 MAT_STATUS;

ros::NodeHandle nh;


void bada_next_state(STATE& present_state);
void bada_roaming(int currentPoint=0);     							// 배회하나 소리가 나면 다음으로 넘어간다.
void bada_go_destination(double x, double y, double orien_z, double orien_w);							// 지정된 방으로 이동.
void bada_save_current_position();					// calculate the person's position on map from robot position using detected angle and theta.
bool bada_rounding();       						// 회전하며 사람이 있는지를 검사한다.
void bada_head_UP_cmd(bool STATUS);						// 카메라달린 모터 위로 들기 for 사람 위치 확인용
int getCurrentRobotPositionTODO();					// get current transform position(pose, quaternion) of robot
void bada_change_pos(float LPos, float APos); 		// 로봇에게 직선거리 혹은 회전 명령 주기 
													// 특정 위치만큼만 이동하기.
													/* 리니어, 앵귤러에 도달할 때까지 회전하도록하기. 기본 속도는 정해져있다. 
													   보내는 것은 cmd_vel, 받는 것은 오도메트리 정보. */
void bada_aligned_pepl(); 							// 사람 찾고 로봇과 사람 위치 정렬하기.
void go_until_touch();							// 버튼이 눌릴 때까지 전진.
void bada_go_to_pepl();
void bada_go_until_touch();
void bada_save_sound_PT();					//로봇의 현재 위치와 소리나는 방향 저장하기.
void bada_go_to_soundPT();   				// 소리가 발생한 지점으로 이동하기.
void bada_display_inform();
void bada_emotion();

void bada_wait_button();

void bada_save_sound_odom();
void bada_go_to_sound();						//소리 발생하는 방향으로 충분히 이동하기.
Position bada_get_robot_pos();

void bada_open_eyes_cmd(bool Status);           					  // Open Eyes Function.
void bada_display_cmd(DISP_EVNT status);                          // Display Command
void bada_vel_cmd(const float XLineVel = 0, const float ZAngleVel = 0);  // commendation of Publishing Velocity


/*--------------------------------------Callback----------------------------------------------*/

void sub_pepl_checker_callback(const geometry_msgs::Point &msg);
void sub_odometry_callback(const nav_msgs::Odometry &msg);
void sub_sig_checker_callback(const std_msgs::Empty &msg);        // Roaming 단계에서 사용. 소리가 발생할 경우에 쓸모가 있다. 
void sub_switch_checker_callback(const std_msgs::Bool &msgs);        // Roaming 단계에서 사용. 소리가 발생할 경우에 쓸모가 있다. 
void sub_sound_localization_callback(const geometry_msgs::PoseStamped &msg);        // 

//==============================================================================================

ros::Publisher pub_cmdvel         	   = nh.advertise<geometry_msgs::Twist>("/cmd_vel", 1);
ros::Publisher pub_camera              = nh.advertise<std_msgs::Bool>("/bada/duino/camera_cmd", 1);
ros::Publisher pub_eyes_open           = nh.advertise<std_msgs::Bool>("/bada/eyes/open", 1);
ros::Publisher pub_head_up             = nh.advertise<std_msgs::Bool>("/bada/duino/camera_cmd", 1);
ros::Publisher pub_display_cmd         = nh.advertise<std_msgs::Int16>("/bada/duino/display_cmd", 1);

ros::Subscriber sub_odometry           = nh.subscribe("/bada/odom", 1, sub_odometry_callback);
ros::Subscriber sub_pepl_checker       = nh.subscribe("/bada/eyes/distance",1, sub_pepl_checker_callback );   //TODO: FIX CALLBACK FUNCTION
ros::Subscriber sub_sig_checker        = nh.subscribe("/bada/signal/checker", 1, sub_sig_checker_callback);     
ros::Subscriber sub_switch_checker     = nh.subscribe("/bada/duino/switch", 1, sub_switch_checker_callback);     
ros::Subscriber sub_sound_localization = nh.subscribe("/bada/signal/localization_filtered", 1, sub_sound_localization_callback);     

//사람이 일정 ROI에 들어오는 것을 검사함. 만약 ROI에 들어온다면, Checker는 True로 바뀜.


typedef actionlib::SimpleActionClient<move_base_msgs::MoveBaseAction> MoveBaseClient;
MoveBaseClient ac("move_base", true); //move_base client 선언

bool PPL_CHECK       = false;
bool SIG_CHECK       = false; 							// Roaming 단계에서 사용. 
bool SWITCH_CHECK    = false; 							// is Switch on?  T/F

ros::Rate loop_rate(6);                      // 과부하방지로 멈추기
//geometry_msgs::Pose2D PERSON_POSITION;
nav_msgs::Odometry CURRENT_ROBOT_POSITION;
geometry_msgs::PoseStamped CURRENT_SOUND_DIRECTION;

struct Position {
	double x;
	double y;
	double orien_z;
	double orien_w;
};
Position SAVED_SOUND_POSITION = {0.0f,1.0f,2.0f,3.0f};
Position SAVED_HUMAN_POSITION =	{0,1,2,3};

double wayPoint[][4] = {
	{1.0,1.0,1.0,0.9},
	{0.5,0.5,0.1,0.4},
	{0.3,2, 0.1,0.2}
};//roaming 장소 저장

int main(int argc, char **argv){
	STATE state = FINDING_PEPL;
	ros::init(argc, argv, "/bada/core");

	bool is_there_pepl = false;

	while (ros::ok()){
		switch (state){
		case FINDING_PEPL:
			do{
				bada_go_destination(0,1,2,3);	            // Go to POINT of ROOM
				bada_head_UP_cmd(true); 				// HEAD_UP
				is_there_pepl = bada_rounding();
				bada_head_UP_cmd(false); 				// HEAD_DOWN
			} while (!is_there_pepl);
			
			bada_next_state(state);
			break;
		case ROAMING:
			bada_roaming();                         // 카테고리에 등록된 소리가 나올 때까지 배회하기
			bada_next_state(state);
			break;
		case SOUND_DETECTING:
			// 소리 난 방향 서브스크라이빙하기
			// 소리 발생한 방향으로 이동하기 
			// 저장하기
			while(true/* enogh */) {
				bada_go_to_sound();						//소리 발생하는 방향으로 충분히 이동하기.
			}					
			bada_save_sound_PT();					//로봇의 현재 위치와 소리나는 방향 저장하기.
			bada_next_state(state);
			break;
		case MOVING_TO_PEPL:
		   	bada_go_to_pepl();  					// 반경 2m 이내 도달 검사하기. 그렇지 않으면 계속 접근
			bada_head_UP_cmd(true); 					// 2m 에 도달하면 카메라 위로 들기
			bada_aligned_pepl();  					// 사람의 위치고 로봇 사람을 가운데로
			bada_go_until_touch(); 			     	// 버튼 눌리기 전까지 전진하기
			bada_change_pos(-5,-5);     			// 뒤로 1m 이동
			bada_change_pos(5,0);      				// 180도 회전 
			bada_next_state(state);
			break;
		case MOVING_WITH_PEPL:
			bada_wait_button();     				// 버튼 눌리기 전까지 대기하기.
			bada_go_to_soundPT();   				// 소리가 발생한 지점으로 이동하기.
			bada_display_inform(); 					// 해당 지점에서 소리
			bada_emotion(); 						// 완료함 표현하기.
			bada_next_state(state);
			break;
		} // END_SWITCH
		ros::spinOnce();
	}	  // END_WHILE
} // END_MAIN


void bada_next_state(STATE& present_state){
	int temp_int = int(present_state);
	STATE temp_state;	
	if (temp_int != int(MOVING_WITH_PEPL)){
		temp_int++;
		temp_state = static_cast<STATE>(temp_int);
		present_state = temp_state;
	}     
	else if(temp_int != int(MOVING_WITH_PEPL)){
		present_state = ROAMING;
	}
}

void bada_go_destination(double x, double y, double orien_z, double orien_w){ //맵 위치 x,y quaternion z,w 를 설정해주고 그 위치로 이동
	//- ACTION MSG 퍼블리시한다.
	ac.waitForServer();
	move_base_msgs::MoveBaseGoal goal;

	goal.target_pose.header.frame_id = "map";
	goal.target_pose.header.stamp = ros::Time::now();

	goal.target_pose.pose.position.x = x;
	goal.target_pose.pose.position.y = y;
	goal.target_pose.pose.orientation.z = orien_z;
	goal.target_pose.pose.orientation.w = orien_w;

	ac.sendGoal(goal);
	return;
}



// void bada_save_current_position(){ //호출되면 로봇 현재 위치 저장
// 	// TODO: use realsense topic to get angle and theta
// 	tf::StampedTransform tranform;
// 	geometry_msgs::Pose2D pose2d;

// 	distance, theta=REALSENSEANGLETHETATODO();
// 	// theta: radian
// 	tranform=getCurrentRobotPositionTODO();
// 	double robotX=transform.pose.x;
// 	double robotY=transform.pose.y;
	
// 	tf::Matrix3x3 m(q);
// 	// https://gist.github.com/marcoarruda/f931232fe3490b7fa20dbb38da1195ac
// 	double roll, pitch, yaw;
// 	m.getRPY(roll, pitch, yaw);
	
//     pose2d.theta = yaw+theta;
// 	double deltaX=distance*cos(pose2d.theta);
// 	double deltaY=distance*sin(pose2d.theta);

//     pose2d.x = robotX+deltaX;
//     pose2d.y = robotY+deltaY;
	
// 	return pose2d;
// }

int getCurrentRobotPositionTODO(){
	// ros::init(argc, argv, "base_link_listener");

	// ros::NodeHandle node;

	// tf::TransformListener listener;

	// tf::StampedTransform tranform;

    // try{
    //   listener.lookupTransform("/map", "/base_link",  
    //                            ros::Time(0), transform);
    // }
    // catch (tf::TransformException ex){
    //   ROS_ERROR("%s",ex.what());
    //   ros::Duration(1.0).sleep();
    // }

	// return transform;
};

bool bada_rounding(){
	geometry_msgs::Twist msg;// 회전하기 위해 퍼블리시 용도로 만들어진 변수
	//subscribing_odometry
	//Save present angle
	// BOOKMARK2
	geometry_msgs::Quaternion initial_angle = CURRENT_ROBOT_POSITION.pose.pose.orientation;        // 현재 각도 정보를 저장
	//http://docs.ros.org/melodic/api/nav_msgs/html/msg/Odometry.html
	
	msg.angular.z = (3.14f/4.0f);      // 회전하도록하기
	
	bada_open_eyes_cmd(true);      // 눈 뜨기. (정보 받기 시작)
    ros::Rate rate(5); // ROS Rate at 5Hz 0.2 sec
	
	int time = 0;
	float Ang_Position = 0;

	do{
		pub_cmdvel.publish(msg); 			//각속도 정보 pub, 통신 실패를 방지하기 위해 while문에 넣어놓음. 
		ros::spinOnce();  				// bada/eyes로부터 토픽 서브스크라이빙, 현재 오도메트리 정보 서브스크라이빙 목적으로 스핀
		msg.angular.z;
		if (PPL_CHECK){		        // 만약 사람 정보가 ROI에 들어왔다면 true
			//BOOKMARK1
			// TODO : USE ROBOT POSITION
			SAVED_HUMAN_POSITION = bada_get_robot_pos();

				/** TODO : 각도와 거리를 이용하여 포인트를 저장한다.  **/
			// ~~맵에 사람의 위치 포인트를 저장하는 방법, 즉 데이터타입이 무엇인지 알아볼 것. ~~<<-- 사람 위치 저장하지 말 것
			// 지금 위치를 저장한다. (로봇의 위치) <<-- 이것을 사용할것
	        break;
		}
		rate.sleep(); 				// 6헤르츠가 적당할 듯. 연산 과부화 방지용.
		time++;
		Ang_Position = (time * 0.2) * msg.angular.z; 
	} while (Ang_Position < 6.28); //한바퀴 돌았는지?
	bada_open_eyes_cmd(false);

	msg.angular.z =0.0; 
	pub_cmdvel.publish(msg); //스탑
	if (PPL_CHECK)
		return true;
	else
		return false;
}

void sub_sig_checker_callback(const std_msgs::Empty &msg){
	SIG_CHECK = true;
} // 소리가 발생하는지 체크하는 콜백함수

void bada_roaming(int currentPoint){ //현재위치에서 마지막 지점까지 이동 
    // bada_go_destination();			        //다음 지점으로 이동하기.
	currentPoint+=1;
	for (int currentPoint; currentPoint<3; currentPoint++){
		bada_go_destination(wayPoint[currentPoint][0],wayPoint[currentPoint][1],wayPoint[currentPoint][2],wayPoint[currentPoint][3]);
			
		while(1){
			ros::spinOnce();                    //소리 검사 결과 받기.
			if (SIG_CHECK) {

				bada_save_sound_odom();	                    //- 소리 정보 오도메트리 저장하기.
				//-  배회 중단. 액션 메시지 취소 보내기
				ac.cancelGoal();
				return;
			}
			if (ac.getState() == actionlib::SimpleClientGoalState::SUCCEEDED /*--목표도착-- ACTION*/) {
				// bada_go_destination();			//다음 지점으로 이동하기.
				break;
			}
		}

	}
}

void bada_go_to_pepl(){

	bada_go_destination(
		SAVED_HUMAN_POSITION.x,
		SAVED_HUMAN_POSITION.y,
		SAVED_HUMAN_POSITION.orien_w,
		SAVED_HUMAN_POSITION.orien_z
	); //사람에게 가기.
	bada_aligned_pepl();
	bada_go_until_touch(); //터치할때까지 전진

	//예전 알고리즘
	// do{
	// 	// 현재 로봇의 위치 받아오기 
		// 
	// }while(/*-이동한 거리가 사람의 반경 2m 이내가 아닐 경우까지 이동. -*/);
	// 	/*- 행동 중단 -*/
	// return;
	// //사람에게 가기.
	// do{
	// 	// 현재 로봇의 위치 받아오기 
	// }while(/*-이동한 거리가 사람의 반경 2m 이내가 아닐 경우까지 이동. -*/);
	// 	/*- 행동 중단 -*/
	// return;
} // END

void bada_aligned_pepl(){
	bada_open_eyes_cmd(true);
	while(false){

		/* velocity.publish 각속도*/
	} //가운데로 맞추기
	// while(/*angle of pepl이 ROI에 도달할 때까지*/){
	// 	/* velocity.publish 각속도*/;
	// } //가운데로 맞추기
	/* velocity.publish  0 */
	// pub_eyes_open.publish(false); 		// 눈 감기. /bada/eyes 작동 종료하도록한다.
}

void bada_go_until_touch(){// 버튼 눌리기 전까지 전진하기
	geometry_msgs::Twist msg;
	msg.linear.x =0.3; 

	do{
		pub_cmdvel.publish(msg); // 앞으로 전진
		ros::spinOnce();
	}while(!SWITCH_CHECK);
	SWITCH_CHECK = false;

	msg.linear.x =0.0; 
	pub_cmdvel.publish(msg); //스탑
} //END

void bada_change_pos(float line, float angle){
    geometry_msgs::Twist vel_cmd2;          // 회전하기 위해 퍼블리시 용도로 만들어진 변수
	do{
		// vel_cmd2.publish();
	} while(true);
}

void sub_odometry_callback(const nav_msgs::Odometry &msg){
	CURRENT_ROBOT_POSITION=msg;
}

// void bada_cancelAllGoal(){
// 	ac.cancelAllGoals();
// }

void bada_go_to_soundPT(){ //사람 데리고 가는용

}


void bada_display_inform(){

}

void bada_emotion(){

}

void bada_wait_button(){

}

void bada_go_to_sound(){ //소리나는 방향으로 이동
	// topic:
	// 
	// - sub sound localization,
	// 
	// - pub cmd vel
	// go to sound while tracking, stop when "enough"

	
}

Position bada_get_robot_pos(){
	Position pos= {CURRENT_ROBOT_POSITION.pose.pose.position.x, CURRENT_ROBOT_POSITION.pose.pose.position.y, CURRENT_ROBOT_POSITION.pose.pose.orientation.z, CURRENT_ROBOT_POSITION.pose.pose.orientation.w};
	return pos;
}

void bada_save_sound_odom(){
	Position pos =bada_get_robot_pos();
	SAVED_SOUND_POSITION= pos;
}
//https://opentutorials.org/module/2894/16661


void bada_open_eyes_cmd(bool status){
	std_msgs::Bool BoolStatus;
	BoolStatus.data = status;
	pub_eyes_open.publish(BoolStatus);
} // True -> Eyes UP, False -> Eyes Down

void bada_head_UP_cmd(bool status){
	std_msgs::Bool BoolStatus;
	BoolStatus.data = status;
	pub_head_up.publish(BoolStatus);
} // True -> UP, False -> DOWN

void bada_display_cmd(DISP_EVNT status){
	std_msgs::Int16 IntStatus;
	IntStatus.data = int(status);
	pub_display_cmd.publish(IntStatus);
}

void bada_vel_cmd(const float XLineVel, const float ZAngleVel){
	geometry_msgs::Twist msg;
	msg.linear.x  = XLineVel; 
	msg.angular.z = ZAngleVel;
	pub_cmdvel.publish(msg);
}

/*--------------------------------------Callback----------------------------------------------*/
void sub_pepl_checker_callback(const geometry_msgs::Point &msg){
	PPL_CHECK = true;
}

void sub_switch_checker_callback(const std_msgs::Bool &msgs){
    SWITCH_CHECK = true;
}

void sub_sound_localization_callback(const geometry_msgs::PoseStamped &msg){
	CURRENT_SOUND_DIRECTION=msg;
}
