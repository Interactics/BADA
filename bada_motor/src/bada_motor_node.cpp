#include <ros/ros.h>
#include <pigpiod_if2.h>
#include <geometry_msgs/Twist.h>
#include <bada_motor/RED.h>

enum MotorPosition{LeftMtr, RightMtr}

const int MOTOR_ENA_L = 17;
const int MOTOR_ENB_L = 4;
const int MOTOR_DIR_L = 24;
const int MOTOR_PWM_L = 13;

const int MOTOR_ENA_R = 22; 
const int MOTOR_ENB_R = 27;
const int MOTOR_DIR_R = 26;
const int MOTOR_PWM_R = 12;

//R_Motor.MotorCtrl(true, 120); // go ahead
//L_Motor.MotorCtrl(false, 120); // go ahead

const int WHEELSIZE = 35;          	// [mm] radius
const int WHEELBASE = 116;         	// [mm] wheel to wheel
const int ENCODER_RESOLUTION = 374; 	// count per cycle

const float MA_KP = 0.8; 
const float MA_KI = 0.3;
const float MA_KD = 0.1;

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

    static void cbf_(int pos);
    
public:

    int current_PWM;
    bool current_Direction;
    int acceleration;

    DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB, int Pi_Num);
    ~DCMotor();

    int EncoderPos();
    void MotorCtrl(bool Dir, int PWM);
    void AccelCtrl(bool Dir, int PWM_needed);
    static void printEncoderPos(int pos);

};

/************************************************************************
*                         Method of DCMotor Class                       *
************************************************************************/

DCMotor::DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB, int Pi_Num)
        : MTR_DIR_(M_Dir), MTR_PWM_(M_PWM), MTR_ENA_(M_encA), MTR_ENB_(M_encB), pinum_(Pi_Num){

    optGlitch_ = 1000;
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
    acceleration = 5; // Acceleration

    ROS_INFO("Setup Fin");
}

DCMotor::~DCMotor(){
      RED_cancel(Encoder_);

}

int DCMotor::EncoderPos(){
    return RED_get_position(Encoder_);
}

void DCMotor::MotorCtrl(bool Dir, int PWM){
    if(Dir == true) { // CW
        gpio_write(pinum_, MTR_DIR_, PI_LOW);
        set_PWM_dutycycle(pinum_, MTR_PWM_, PWM);
        current_PWM = PWM;
        current_Direction = true;
    } else { // CCW
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
    } else if (PWM_desired < current_PWM) {
        PWM_local = current_PWM - acceleration;
        MotorCtrl(Dir, PWM_local);
    } else {
        PWM_local = current_PWM;
        MotorCtrl(Dir, PWM_local);
    }
    //ROS_INFO("Current_PWM is %d", current_PWM);
}

void DCMotor::cbf_(int pos){
    std::cout << pos << std::endl;
}

//==================================DCMotor Class========================= 
//========================================================================
/////////////////////////////////////////////////////////////////////////////////////


//if it were connected with pigpiod, 0 value is returend.

int pinum = pigpio_start(NULL, NULL);

DCMotor R_Motor = DCMotor(MOTOR_DIR_R, MOTOR_PWM_R, MOTOR_ENA_R, MOTOR_ENB_R, pinum);
DCMotor L_Motor = DCMotor(MOTOR_DIR_L, MOTOR_PWM_L, MOTOR_ENA_L, MOTOR_ENB_L, pinum);


void Initialize(){

    PWM_Limit = 250;
    switch_direction = true;
    Theta_Distance_Flag = 0;
    ROS_INFO("Initialize Complete");
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
/*
void MotorSpeedCtrl(DCMotor motor){
    float err = 0;
    float up =0, ui =0, ud =0;
    float input_u = 0;
    int u_val = 0;
    bool m_dir = 0;
    static float err_k_1 = 0;
    static float err_sum =0;
    
    err = speed_ref_a - speed_a; 
    err_sum += err;

    up = KP * err;
    ui = KI * err_sum;
    ud = KD * (err - err_k_1);

    err_k1 = motor.EncoderPos();
    
    input_u = up + ui + ud;

    if(input_u < 0){
        m_dir = 1;
        input_u *= -1;
    }
    else{
	m_dir = 0;
    }

    if (input_u > 255) u_val - 255;
    else u_val = input_u;
}
*/
//void calculate_RPM(int time, Motor* ENC){
//
//}

//TIME Interrupt
volatile bool t100ms_flag = false;
int t100ms_index = 0;

void timer_CB(const ros::TimerEvent &event){
	t100ms_flag = true;
}

int encoderR_now = 0;
int encoderL_now = 0;
int encoderR_prev = 0;
int encoderL_prev = 0;
int encoderR_diff = 0;
int encoderL_diff = 0;
double RPM_R = 0; // [ROUND per min]
double RPM_L = 0;
double Vel_R = 0; // [m/s]
double Vel_L = 0;
double aVel_R = 0; // anguler velocities [Rad/sec]
double aVel_L = 0; 
double Linear_Vel = 0; // [m/s]
double Angular_Vel = 0; // [Rad/sec]

double VelEnc_Transform(double vel){
	double temp_RPM;
	temp_RPM = double(vel * 60)/double(2*PI*WHEELSIZE) * 1000;
	return double(temp_RPM * ENCODER_RESOLUTION) / 600; //ENC
}

double EncVel_Transform(int diff_Enc){
	double temp_RPM;
    temp_RPM = double(diff_Enc) / 100 * 60 / 1 * 1000 / 1 * 1 / ENCODER_RESOLUTION; 
    return temp_RPM / 1 * 1 / 60 * 2*WHEELSIZE*PI / 1 * 1 / 1000; // Velocity
}


void PIDCtrl(float TargetSpd, float Spd){
	//Spd = -Spd;
	
    float err = 0;
    float uP =0, uI = 0, uD =0;
    float input_u = 0;
    int u_val = 0;
    bool m_dir = 0;

    static float err_k_1 = 0;
    static float err_sum = 0;
    
	double TargetENC;
	double ENC;
	double input_spd;
	
	
	TargetENC = VelEnc_Transform(TargetSpd);
	ENC = VelEnc_Transform(Spd);
    

    err = TargetENC - ENC;
    err_sum += err;
    
    uP = MA_KP * err;
    uI = MA_KI * err_sum;
    uD = MA_KD * (err - err_k_1);
    
    err_k_1 = err;
    input_u = uP + uI + uD;
    std::cout << " \n" << "TargetENC : " << TargetENC << " ENC : " << ENC << " input_u : " << input_u << std::endl;	
    if(input_u < 0 ){
        m_dir = false ;
        input_u *= -1;
    }
    else {
        m_dir = true    ;
    }

    if (input_u > 150) u_val = 150;
    else u_val = input_u;
    
    input_spd = EncVel_Transform(u_val);
    
    std::cout << "ERR : " << err << " u_val : " << u_val << " INput : " << input_spd << m_dir << std::endl;
    
    R_Motor.MotorCtrl(m_dir, u_val);
    
//R_Motor.MotorCtrl(true, 120); // go ahead
//L_Motor.MotorCtrl(false, 120); // go ahead
}



void BadaCtrl(float L, float R){

}

int main (int argc, char **argv) {
    ros::init(argc, argv, "bada");
    ros::NodeHandle nh;

    ROS_INFO("---BADA Node Start---");
    Initialize();

    ros::Timer timer =nh.createTimer(ros::Duration(0.01), timer_CB);
    geometry_msgs::Twist cmd_vel;

    ros::Publisher bada_cmd_vel = nh.advertise<geometry_msgs::Twist>("/bada/cmd_vel", 1);
    ros::Rate loop_rate(10);
    ros::Time begin = ros::Time::now();

    while(ros::ok()) {
        //std::cout << R_Motor.EncoderPos() << std::endl;
        //std::cout << L_Motor.EncoderPos() << std::endl;
	//std::cout << ros::Time::now() << std::endl;
	ros::spinOnce();
	


	if(t100ms_flag){
		t100ms_flag = false;	
 
        switch(t100ms_index){
	    case 0: 
		    //ROS_INFO("time is %d", t100ms_index); 

		    t100ms_index = 1 ;
		    break;
	    case 1: 
		    //ROS_INFO("time is %d", t100ms_index); 

		    t100ms_index = 2;
		    break;
	    case 2: 
	 	    //ROS_INFO("time is %d", t100ms_index);
		   //Wheel Spd(50msec) 
		    encoderR_now = R_Motor.EncoderPos();
		    encoderL_now = L_Motor.EncoderPos();

		    encoderR_diff = encoderR_now - encoderR_prev;
		    encoderL_diff = -(encoderL_now - encoderL_prev);

		    encoderR_prev = encoderR_now;
		    encoderL_prev = encoderL_now;

		   
		    t100ms_index = 3;
		    break;
	    case 3: 
		    //ROS_INFO("time is %d", t100ms_index); 
		    RPM_R = double(encoderR_diff) / 100 * 60 / 1 * 1000 / 1 * 1 / ENCODER_RESOLUTION; 
	        RPM_L = double(encoderL_diff) / 100 * 60 / 1 * 1000 / 1 * 1 / ENCODER_RESOLUTION;
	        // RPM = [del(encoder)/ms] * [1000ms/1sec] * [60sec/1 min] * [1round/1roundEncoder]
	        
            std::cout << "R : "<<RPM_R << "   L : " <<  RPM_L << std::endl;
		    t100ms_index = 4;
		    break;
	    case 4: 
		    //ROS_INFO("time is %d", t100ms_index);
		     
		    Vel_R = RPM_R / 1 * 1 / 60 * 2*WHEELSIZE*PI / 1 * 1 / 1000;
		    Vel_L = RPM_L / 1 * 1 / 60 * 2*WHEELSIZE*PI / 1 * 1 / 1000;
		    
		    //Velocity = [(RPM)Round/1min] * [1min/60sec] * [2pi*r/1Round] * [1m/1000sec]
	        
	        aVel_R = Vel_R / (2 * WHEELSIZE);
		    aVel_L = Vel_L / (2 * WHEELSIZE);

		    Linear_Vel  = (Vel_R + Vel_L) / 2;
		    Angular_Vel = (Vel_R - Vel_L) / WHEELBASE * 1000 ;
		    
		    ROS_INFO("velocity : %.2f m/s     Ang : %.2f rad", Linear_Vel, Angular_Vel);
		    ROS_INFO("Vel_L : %.2f m/s \t    Vel_R : %.2f m/s", Vel_L, Vel_R);
		    PIDCtrl(0.15, Vel_R);
		    
		    
		    t100ms_index = 5;
		    break;
	    case 5: 
		    //ROS_INFO("time is %d", t100ms_index);
		    //Publish


		    cmd_vel.linear.x = Linear_Vel;
		    cmd_vel.angular.z = Angular_Vel;

		    bada_cmd_vel.publish(cmd_vel);
		    t100ms_index = 6;
 		    break;
	    case 6: 
		    //ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 7;
		    break;
	    case 7: 
		    //ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 8;
		    break;
	    case 8: 
		    //ROS_INFO("time is %d", t100ms_index); 
		    //R_Motor.MotorCtrl(true, 120); // go ahead
		    //L_Motor.MotorCtrl(false, 120); // go ahead

		    t100ms_index = 9;
		    break;
	    case 9:
		    //ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 0;
		    break;
        defalut:
		    //ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 0;
		    break;
	    }
   	    
	}
	//Theta_Turn(90, 100);
        //loop_rate.sleep();
    }

    R_Motor.MotorCtrl(true, 0);
    L_Motor.MotorCtrl(true, 0);
    pigpio_stop(pinum);

    return 0;
}


