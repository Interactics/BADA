#include <ros/ros.h>
#include <geometry_msgs/Twist.h>
#include <nav_msgs/Odometry.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Int16.h>
#include <std_msgs/Empty.h>

//---------------- 노드 변경할 것 ----------------------------------------
	/*
	TODO
	1. bada_eyes 변경할 것
	  bada_eyes에 활성화 여부 서브스크라이빙 기능으로 bool type 추가하기
	  bada_eyes에 퍼블리싱하는 것으로 각도, 거리, 중앙포인트의 좌표 추가하기 
	2. 목표 거리 이동, 목표 회전치 이동 함수 만들어서 테스트해볼것
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

std_msgs::Bool HEAD_STATUS;
std_msgs::Int16 MAT_STATUS;

ros::NodeHandle nh;

ros::Publisher bada_cmdvel   = nh.advertise<std_msgs::Bool> ("/cmd_vel", 1);
ros::Publisher bada_camera   = nh.advertise<std_msgs::Bool> ("/bada/duino/camera_cmd", 1);
ros::Publisher bada_display  = nh.advertise<std_msgs::Int16>("/bada/duino/display_cmd", 1);
ros::Subscriber pepl_checker = nh.subscribe<std_msgs::Int32>("/bada/eyes/distance",1, ?? );        
//사람이 일정 ROI에 들어오는 것을 검사함. 만약 ROI에 들어온다면, Checker는 True로 바뀜.

ros::Subscriber sig_checker  = nh.subscribe<std_msgs::Empty>("/bada/signal/checker", 1, sig_income);     

bool sig_checker = false; 							// Roaming 단계에서 사용. 
void sig_income(const std_msgs::Empty &msg);        // Roaming 단계에서 사용. 소리가 발생할 경우에 쓸모가 있다. 
void bada_roaming();     							// 배회하나 소리가 나면 다음으로 넘어간다.
void bada_go_destination();							// 지정된 방으로 이동.
void bada_rounding();       						// 회전하며 사람이 있는지를 검사한다.
void bada_head_UP(bool STATUS);						// 카메라달린 모터 위로 들기 for 사람 위치 확인용
bool bada_change_pos(float LPos, float APos); 		// 로봇에게 직선거리 혹은 회전 명령 주기 
													// 특정 위치만큼만 이동하기.
													/*리니어, 앵귤러에 도달할 때까지 회전하도록하기. 기본 속도는 정해져있다. 
													보내는 것은 cmd_vel, 받는 것은 오도메트리 정보. */


int main(int argc, char **argv){
	STATE state = FINDING_PEPL;
	ros::init(argc, argv, "/bada/core");
    ros::Rate loop_rate(6);

	while (ros::ok()){
		switch (state){
		case FINDING_PEPL:
			bool is_there_pepl = false;
			do{
				bada_go_destination();	            // Go to POINT of ROOM
				bada_head_UP(true); 				// HEAD_UP
				is_there_pepl = bada_rounding();
				bada_head_UP(false); 				// HEAD_DOWN
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
			while(/* enogh */) {
		    bada_sub_sound_odom();					//소리 발생하는 방향 인지하기
			bada_go_to_sound();						//소리 발생하는 방향으로 충분히 이동하기.
			}					
			bada_save_sound_PT();					//로봇의 현재 위치와 소리나는 방향 저장하기.
			bada_next_state(state);
			break;
		case MOVING_TO_PEPL:
		   	bada_go_to_pepl();  					// 반경 2m 이내 도달 검사하기. 그렇지 않으면 계속 접근
			bada_head_UP(true); 					// 2m 에 도달하면 카메라 위로 들기
			bada_check_pepl();  					// 사람의 위치 잡기
			bada_change_pos();  					// 사람을 가운데로 오게 하여금 로봇 회전하기
			bada_go_while_button(); 				// 버튼 눌리기 전까지 전지하기
			bada_change_pos();     					// 뒤로 1m 이동
			bada_change_pos();      				// 180도 회전 
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
	if (present_state != MOVING_WITH_PEPL)        present_state++;
	else if(present_state == MOVING_WITH_PEPL)    present_state = ROAMING;
	//---끝---
}

void bada_go_destination(){
	//- ACTION MSG 퍼블리시한다.
	//---끝---
}

void bada_head_UP(bool STATUS){
	bada_camera.publish(STATUS);
}

bool bada_rounding(){
	bool pepl_detected;
    stds_msgs::twist vel_cmd;          // 회전하기 위해 퍼블리시 용도로 만들어진 변수
	//subscribing_odometry
	//Save present angle
	bada_odometry = init_angle;        // 현재 각도 정보를 저장
	vel_cmd.angularvel.z(3.14/4);      // 회전하도록하기
	ros::rate(6);                      // 과부하방지로 멈추기
	bada_eyes_open.publish(true);      // 눈 뜨기. (정보 받기 시작)

	do{
		bada_cmdvel.publish(); 			//각속도 정보 pub, 통신 실패를 방지하기 위해 while문에 넣어놓음. 
		ros::spinOnce();  				// bada/eyes로부터 토픽 서브스크라이빙, 현재 오도메트리 정보 서브스크라이빙 목적으로 스핀
		if (pepl_detected){		        // 만약 사람 정보가 ROI에 들어왔다면 true
			bada_save_pt_of_pepl(); 
			/** TODO : 각도와 거리를 이용하여 포인트를 저장한다.  **/
			// ~~맵에 사람의 위치 포인트를 저장하는 방법, 즉 데이터타입이 무엇인지 알아볼 것. ~~<<-- 사람 위치 저장하지 말 것
			// 지금 위치를 저장한다. (로봇의 위치) <<-- 이것을 사용할것
	        break;
		}
		loop_rate.sleep(); 				// 6헤르츠가 적당할 듯. 연산 과부화 방지용.
	} while ((bada_odometry.angle - init_angle) > 360; //한바퀴 돌았는지?
	bada_eyes_open.publish(false); 		// 눈 감기. /bada/eyes 작동 종료하도록한다.
	bada_cmdvel.publish(vel);			// velocity pub 0, 0  로봇 회전 중단.
	if (pepl_detected)
		return true;
	else
		return false;
	//---끝---
}

void sig_income(const std_msgs::Empty &msg){
	sig_checker = true;
} // 소리가 발생하는지 체크하는 콜백함수

void bada_roaming(){
    bada_go_destination();			        //다음 지점으로 이동하기.
	while(1){
		ros::spinOnce();                    //소리 검사 결과 받기.
		if (sig_checker) break;
		if (/*목표도착*/) {
			bada_go_destination();			//다음 지점으로 이동하기.
		}
	}
	save_sound_odom();	                    //- 소리 정보 오도메트리 저장하기.
	//-  배회 중단. 액션 메시지 취소 보내기
}