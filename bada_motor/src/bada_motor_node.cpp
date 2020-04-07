#include <ros/ros.h>
#include <pigpiod_if2.h>
#include <bada_motor/RED.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <cmath>

enum MotorPosition{LEFT, RIGHT};

const int MOTOR_ENA_L = 17;
const int MOTOR_ENB_L = 4;
const int MOTOR_DIR_L = 24;
const int MOTOR_PWM_L = 13;

const int MOTOR_ENA_R = 22; 
const int MOTOR_ENB_R = 27;
const int MOTOR_DIR_R = 26;
const int MOTOR_PWM_R = 12;

const int WHEELSIZE = 35;          	    // [mm] radius
const int WHEELBASE = 116;         	    // [mm] wheel to wheel
const int ENCODER_RESOLUTION = 374; 	// count per cycle

const double PI = 3.141592;

int PWM_Limit;

volatile int EncoderCounter1;
volatile int EncoderCounter2;

bool switch_direction;
int Theta_Distance_Flag;

//----------------------------------------------------------------------------------
/************************************************************************
*                            DCMotor Class                              *
************************************************************************/

class DCMotor{
private:
	MotorPosition MotorPosition_;
	
    int nPin_;

    int MTR_ENA_;
    int MTR_ENB_;
    int MTR_DIR_;
    int MTR_PWM_;

    int PWM_range_;
    int PWM_frequency_;
    int PWM_currunt_;
    int pinum_;

    int optGlitch_;
    int optMode_;
    RED_t* Encoder_;  //Encoder Setting    
    
    //For PID
    float P_gain_;
	float I_gain_;
	float D_gain_;
    
    float err_k_1_;
    float err_sum_;

    static void cbf_(int pos);

    //encoder Cal 
    int Pos_now;
    int Pos_diff;
    int Pos_prev;

    //PID Velocity Target
    double Vel_Target_;

public:
    double VELOCITY;
    int current_PWM;
    bool current_Direction;
    int acceleration;

    DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB, int Pi_Num, MotorPosition MPosition);
    ~DCMotor();

    int EncoderPos();
    void MotorCtrl(bool Dir, int PWM);
    void AccelCtrl(bool Dir, int PWM_needed);
    static void printEncoderPos(int pos);
    void PIDgainSET(float P_gain, float I_gain, float D_gain );
    void SetVel_Target(double Vel);
    void PIDCtrl_(float TargetSpd);
    double VelEnc_Transform_(double vel);
    double EncVel_Transform_(int diff_Enc);
    void EncoderDiff();
    void CalVel();
    void PIDUpdate();
};

/************************************************************************
*                         Method of DCMotor Class                       *
************************************************************************/

DCMotor::DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB, int Pi_Num, MotorPosition MPosition)
        : MTR_DIR_(M_Dir), MTR_PWM_(M_PWM), MTR_ENA_(M_encA), MTR_ENB_(M_encB), pinum_(Pi_Num),
          MotorPosition_(MPosition) 
{
    optGlitch_ = 100;
    optMode_ = RED_MODE_DETENT;
    PWM_range_ = 512;          
    PWM_frequency_ = 40000;    

    Encoder_ = RED(pinum_, MTR_ENA_, MTR_ENB_, optMode_, NULL);

    RED_set_glitch_filter(Encoder_, optGlitch_);  

    set_mode(Pi_Num, MTR_DIR_, PI_OUTPUT);
    set_mode(Pi_Num, MTR_PWM_, PI_OUTPUT);

    set_PWM_range(Pi_Num, MTR_PWM_, PWM_range_);
    set_PWM_frequency(Pi_Num, MTR_PWM_, PWM_frequency_);

    gpio_write(Pi_Num, MTR_DIR_, PI_LOW);
    set_PWM_dutycycle(Pi_Num, MTR_PWM_, 0);

    current_PWM = 0;
    current_Direction = true;
    acceleration = 5; 
    // Acceleration
    
    //For PID
    err_k_1_ = 0;
    err_sum_ = 0;

    //For Encoder Diff
	Pos_now =0;
    Pos_diff = 0;
	Pos_prev = 0;

    ROS_INFO("Setup Fin");
}

DCMotor::~DCMotor(){
      RED_cancel(Encoder_);
}

int DCMotor::EncoderPos(){
    return RED_get_position(Encoder_);
}

double DCMotor::VelEnc_Transform_(double vel){
	double temp_RPM;
	temp_RPM = double(vel * 60)/double(2*PI*WHEELSIZE) * 1000;
	return double(temp_RPM * ENCODER_RESOLUTION) / 600; //ENC
}

double DCMotor::EncVel_Transform_(int diff_Enc){
// RPM = [del(encoder)/ms] * [1000ms/1sec] * [60sec/1 min] * [1round/1roundEncoder]
// Velocity = [(RPM)Round/1min] * [1min/60sec] * [2pi*r/1Round] * [1m/1000sec]
	double temp_RPM;
    temp_RPM = double(diff_Enc) / 100 * 60 / 1 * 1000 / 1 * 1 / ENCODER_RESOLUTION; 
    return temp_RPM / 1 * 1 / 60 * 2*WHEELSIZE*PI / 1 * 1 / 1000; // Velocity
}

void DCMotor::MotorCtrl(bool Dir, int PWM){
    if(Dir == true) { // CW
        gpio_write(pinum_, MTR_DIR_, PI_LOW);
        set_PWM_dutycycle(pinum_, MTR_PWM_, PWM);
        current_PWM = PWM;
        current_Direction = true;
    } 
    else { // CCW
		gpio_write(pinum_, MTR_DIR_, PI_HIGH);
		set_PWM_dutycycle(pinum_, MTR_PWM_, PWM);
		current_PWM = PWM;
		current_Direction = false;
    } 
}

void DCMotor::AccelCtrl(bool Dir, int PWM_desired){
    // P ctrl
    int PWM_local;
    
    if(PWM_desired > current_PWM) {
        PWM_local = current_PWM + acceleration;
        MotorCtrl(Dir, PWM_local);
    } 
    else if (PWM_desired < current_PWM) {
        PWM_local = current_PWM - acceleration;
        MotorCtrl(Dir, PWM_local);
    } 
    else {
        PWM_local = current_PWM;
        MotorCtrl(Dir, PWM_local);
    }
    //ROS_INFO("Current_PWM is %d", current_PWM);
}

void DCMotor::cbf_(int pos){
    std::cout << pos << std::endl;
}

void DCMotor::EncoderDiff(){
	Pos_now  = EncoderPos();
	Pos_diff = Pos_now - Pos_prev;	
	if (MotorPosition_ == LEFT)
        Pos_diff = -1*Pos_diff;
	Pos_prev = Pos_now;
}

void DCMotor::CalVel(){
    VELOCITY = EncVel_Transform_(Pos_diff);
}

void DCMotor::PIDgainSET(float P_gain, float I_gain, float D_gain ){
	P_gain_ = P_gain;
	I_gain_ = I_gain;
	D_gain_ = D_gain;
}

void DCMotor::PIDCtrl_(float TargetSpd){
    float Spd = VELOCITY;
    float err = 0;
    float uP = 0, uI = 0, uD =0;
    float input_u = 0;
    
    int u_val = 0;
    bool m_dir = 0;

	double TargetENC;
	double ENC;
	double input_spd;
	
	TargetENC = 	VelEnc_Transform_(TargetSpd);
	ENC 	  = 	VelEnc_Transform_(Spd);

    err = 			TargetENC - ENC;
    err_sum_ += 	err;
    
    uP = 			P_gain_ * err;
    uI = 			I_gain_ * err_sum_;
    uD = 			D_gain_ * (err - err_k_1_);
    	
    err_k_1_ = err;
    input_u = uP + uI + uD;

    
    if(MotorPosition_ == RIGHT){
		if(input_u < 0 ){
		    m_dir = false ;
		    input_u *= -1;
		}
		else {
		    m_dir = true;
		}
	}
	
    if(MotorPosition_ == LEFT){
		if(input_u < 0 ){
		    m_dir = true ;
		    input_u *= -1;
		}
		else {
		    m_dir = false;
		}
	}

    if (input_u > 255) u_val = 255;
    else u_val = input_u;
    
    input_spd = EncVel_Transform_(u_val);

    if(MotorPosition_ == LEFT) std::cout << "--------LEFT--------" << std::endl;
    else if(MotorPosition_ == RIGHT) std::cout << "--------RIGHT--------" << std::endl;
    std::cout << "TargetENC : " << TargetENC << " ENC : " << ENC << 
    " input_u : " << input_u << std::endl;
    std::cout << "ERR : " << err << " u_val : " << u_val 
    << " input_spd : " << input_spd 
    << std::endl << " m_dir : " << m_dir << std::endl;
    
    std::cout << "---------------------------" << std::endl << std::endl;
    MotorCtrl(m_dir, u_val);
}

void DCMotor::SetVel_Target(double Vel){
    Vel_Target_ = Vel;
}

void DCMotor::PIDUpdate(){
    PIDCtrl_(Vel_Target_);
}

//==================================DCMotor Class========================= 
//========================================================================
/////////////////////////////////////////////////////////////////////////////////////


//if it were connected with pigpiod, 0 value is returend.
int pinum = pigpio_start(NULL, NULL);

DCMotor R_Motor = DCMotor(MOTOR_DIR_R, MOTOR_PWM_R, MOTOR_ENA_R, MOTOR_ENB_R, pinum, RIGHT);
DCMotor L_Motor = DCMotor(MOTOR_DIR_L, MOTOR_PWM_L, MOTOR_ENA_L, MOTOR_ENB_L, pinum, LEFT) ;

void Initialize(){
    PWM_Limit = 255;
    switch_direction = true;
    Theta_Distance_Flag = 0;

    R_Motor.PIDgainSET(0.8, 0.3, 0.1);
	L_Motor.PIDgainSET(0.8, 0.3, 0.1);

    ROS_INFO("Initialize Complete");
    ROS_INFO("---BADA Node Start---");
}


int Limit_Function(int pwm){
    int output;
    if(pwm > PWM_Limit)     output = PWM_Limit;
    else if(pwm < 0)        output = 0;
    else                    output = pwm;
    return output; 
}

void Theta_Turn (float Theta, int PWM){
    double local_encoder;
    int local_PWM = Limit_Function(PWM);
    if(Theta_Distance_Flag == 1) {
        EncoderCounter1 = 0;
        EncoderCounter2 = 0;
        Theta_Distance_Flag = 2;
    }
    if(Theta > 0) {
        local_encoder = Theta; //(Round_Encoder/360)*(Robot_Round/Wheel_Round)
        R_Motor.MotorCtrl(true, local_PWM);
        L_Motor.MotorCtrl(true, local_PWM);
        //motor1.Accel_Controller(true, local_PWM);
        //motor2.Accel_Controller(true, local_PWM);
    }
    else {
        local_encoder = -Theta; //(Round_Encoder/360)*(Robot_Round/Wheel_Round)
        R_Motor.MotorCtrl(false, local_PWM);
        L_Motor.MotorCtrl(false, local_PWM);
        //motor1.Accel_Controller(false, local_PWM);
        //motor2.Accel_Controller(false, local_PWM);
    }

    if(EncoderCounter1 > local_encoder) {
        //ROS_INFO("Encoder A1 is %d", EncoderCounter1);
        //ROS_INFO("Encoder A2 is %d", EncoderCounter2);
        EncoderCounter1 = 0;
        EncoderCounter2 = 0;
        R_Motor.MotorCtrl(true, 0);
        L_Motor.MotorCtrl(true, 0);
        //motor1.Motor_Controller(true, 0);
        //motor2.Motor_Controller(true, 0);
        Theta_Distance_Flag = 3;
    }
}

void TwistToMotor_CB(const geometry_msgs::Twist &msg){
	
    double dx = 0, dy = 0, dr = 0;
    double RIGHT_V = 0, LEFT_V = 0; 

    dx = msg.linear.x;
    dy = msg.linear.y;
    dr = msg.angular.z;
   
    RIGHT_V = dx + dr * WHEELBASE / 2000;
    LEFT_V = dx - dr * WHEELBASE / 2000;
    
    R_Motor.SetVel_Target(RIGHT_V);
    L_Motor.SetVel_Target(LEFT_V);
    //R_Motor.PIDCtrl_(RIGHT_V);
    //L_Motor.PIDCtrl_(LEFT_V); 

}

//TIME Interrupt
volatile bool t100ms_flag = false;
int t100ms_index = 0;

void timer_CB(const ros::TimerEvent &event){
	t100ms_flag = true;
}

double Vel_R = 0; 			// [m/s]
double Vel_L = 0;
double aVel_R = 0; 			// anguler velocities [Rad/sec]
double aVel_L = 0; 
double Linear_Vel = 0; 		// [m/s]
double Angular_Vel = 0; 	// [Rad/sec]
double x_dist, y_dist = 0;
double Angle_delta = 0, Angle = 0;
double Distance_delta = 0;




int main (int argc, char **argv) {
    ros::init(argc, argv, "bada");
    ros::NodeHandle nh;
    Initialize();
    
    ros::Time current_time;

    ros::Timer timer = nh.createTimer(ros::Duration(0.01), timer_CB);
    geometry_msgs::Twist cmd_vel;
    
    //Odometry Inform
    ros::Publisher bada_odom = nh.advertise<nav_msgs::Odometry>("/bada/odom", 1);
    ros::Subscriber sub_vel = nh.subscribe("cmd_vel", 1, TwistToMotor_CB);
    tf::TransformBroadcaster odom_broadcaster;

//    ros::Publisher bada_cmd_vel = nh.advertise<geometry_msgs::Twist>("/bada/cmd_vel", 1); 


    ros::Time begin = ros::Time::now();
    ros::Rate loop_rate(10);
   
    nav_msgs::Odometry odom;
    geometry_msgs::Quaternion odom_quat;
    geometry_msgs::TransformStamped odom_trans;
    while(ros::ok()) {
		ros::spinOnce();
		if(t100ms_flag){
			t100ms_flag = false;	
			
		    switch(t100ms_index){
			case 0: 
				t100ms_index = 1 ;
				break;
			case 1: 
				t100ms_index = 2;
				break;
			case 2: 
				R_Motor.EncoderDiff();
				L_Motor.EncoderDiff();
				t100ms_index = 3;
				break;
			case 3: 
				t100ms_index = 4;
				break;
			case 4: 
				R_Motor.CalVel();
				L_Motor.CalVel();

				Vel_R = R_Motor.VELOCITY;
				Vel_L = L_Motor.VELOCITY;

				aVel_R = Vel_R / (2 * WHEELSIZE);
				aVel_L = Vel_L / (2 * WHEELSIZE);

				Linear_Vel  = (Vel_R + Vel_L) / 2;
				Angular_Vel = (Vel_R - Vel_L) / WHEELBASE * 1000 ;
				
				ROS_INFO("Linear : %.2f m/s\tAng : %.2f rad", Linear_Vel, Angular_Vel);
				ROS_INFO("Vel_L : %.2f m/s \tVel_R : %.2f m/s", Vel_L, Vel_R);

		           	//R_Motor.PIDCtrl_(0.30);
			        //L_Motor.PIDCtrl_(0.30);
			        R_Motor.PIDUpdate();
				L_Motor.PIDUpdate();

				t100ms_index = 5;
				break;
			case 5: //Publish Part
				cmd_vel.linear.x = Linear_Vel;
				cmd_vel.angular.z = Angular_Vel;
				//bada_cmd_vel.publish(cmd_vel);
                                //ros::spinOnce();				
				t100ms_index = 6;
	 		    break;
			case 6:
			        Distance_delta = Linear_Vel*0.1;
			        Angle_delta = Angular_Vel*0.1;

				x_dist = x_dist + Distance_delta * cos(Angle + Angle_delta/2);
			        y_dist = y_dist + Distance_delta * sin(Angle + Angle_delta/2);
				Angle = Angle + Angle_delta;
				
				std::cout << "(x, y) = (" << x_dist << ", " << y_dist << ")" << std::endl;	
  				t100ms_index = 7;
				break;
			case 7: 
				current_time = ros::Time::now();
				//compute odom of robot 
				odom_quat = tf::createQuaternionMsgFromYaw(Angle);

				//Pub TF
				odom_trans.header.stamp = current_time;
				odom_trans.header.frame_id = "odom";
				odom_trans.child_frame_id = "bada_chasis";

				odom_trans.transform.translation.x = x_dist;
				odom_trans.transform.translation.y = y_dist;
				odom_trans.transform.translation.z = 0.0;
				odom_trans.transform.rotation = odom_quat;
				
				//Send the TF
				odom_broadcaster.sendTransform(odom_trans);

				//Pub Odometry 
				odom.header.stamp = current_time;
				odom.header.frame_id = "odom";

				odom.pose.pose.position.x = x_dist;
				odom.pose.pose.position.y = y_dist;
				odom.pose.pose.position.z = 0.0;
				odom.pose.pose.orientation = odom_quat;

				//set the velocity
				odom.child_frame_id = "bada_chasis";
				odom.twist.twist.linear.x  = Linear_Vel;
				odom.twist.twist.linear.y  = 0.0 ;
				odom.twist.twist.angular.z = Angular_Vel;

				bada_odom.publish(odom);

				t100ms_index = 8;
				break;
			case 8: 
				t100ms_index = 9;
				break;
			case 9:
				t100ms_index = 0;
				break;
		    defalut:
				t100ms_index = 0;
				break;
			}
		}
    }

    R_Motor.MotorCtrl(true, 0);
    L_Motor.MotorCtrl(true, 0);
    pigpio_stop(pinum);
    return 0;
} 


