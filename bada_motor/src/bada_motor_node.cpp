#include <ros/ros.h>
#include <pigpiod_if2.h>
#include <bada_motor/RED.h>

const int MOTOR_DIR_R = 26;
const int MOTOR_PWM_R = 12;
const int MOTOR_ENA_R = 17;
const int MOTOR_ENB_R = 4;

const int MOTOR_DIR_L = 19;
const int MOTOR_PWM_L = 13;
const int MOTOR_ENA_L = 22; 
const int MOTOR_ENB_L = 27;

const int WHEELSIZE = 50; // mm
const int WHEELBASE = 100; //mm
const int ENCODER_RESOLUTION = 370;

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

    PWM_Limit = 150;
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


int main (int argc, char **argv) {
    ros::init(argc, argv, "motor_node");
    ros::NodeHandle nh;

    ROS_INFO("---BADA Node Start---");
    Initialize();

    ros::Timer timer =nh.createTimer(ros::Duration(0.01), timer_CB);

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
		    ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 1 ;
		    break;
	    case 1: 
		    ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 2;
		    break;
	    case 2: 
		   //Wheel Spd(50msec) 
		    encoderR_now = R_Motor.EncoderPos();
		    encoderL_now = L_Motor.EncoderPos();

		    encoderR_diff = encoderR_now - encoderR_prev;
		    encoderL_diff = encoderL_now - encoderL_prev;

		    encoderR_prev = encoderR_now;
		    encoderL_prev = encoderL_now;
	 	    ROS_INFO("time is %d", t100ms_index);
		   
		    t100ms_index = 3;
		    break;
	    case 3: 
		    ROS_INFO("time is %d", t100ms_index); 
		    RPM_R = double(encoderR_diff)/100 * 60 / 1 * 1000 / 1 * 1 / ENCODER_RESOLUTION; 
	            RPM_L = double(encoderL_diff)/100 * 60 / 1 * 1000 / 1 * 1 / ENCODER_RESOLUTION;
                    std::cout << RPM_R << std::endl;
		    t100ms_index = 4;
		    break;
	    case 4: 

		    Vel_R = RPM_R / 1 * 1 / 60 * 2 * WHEELSIZE * PI / 1 * 1 / 1000;
		    Vel_L = RPM_L / 1 * 1 / 60 * 2 * WHEELSIZE * PI / 1 * 1 / 1000;
	            aVel_R = Vel_R / (2*WHEELSIZE);
		    aVel_L = Vel_L / (2*WHEELSIZE);

		    Linear_Vel  = (Vel_R + Vel_L) / 2;
		    Angular_Vel = (Vel_R - Vel_L) / WHEELBASE * 1000 ;

		    ROS_INFO("velocity : %.2f m/s ", Angular_Vel);
		    ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 5;
		    break;
	    case 5: 
		    ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 6;
 		    break;
	    case 6: 
		    ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 7;
		    break;
	    case 7: 
	        /*    encoderR_now = R_Motor.EncoderPos();
		    encoderL_now = L_Motor.EncoderPos();

		    encoderR_diff = encoderR_now - encoderR_prev;
		    encoderL_diff = encoderL_now - encoderL_prev;

		    encoderR_prev = encoderR_now;
		    encoderL_prev = encoderL_now;
	 	    
		    std::cout << encoderR_diff << std::endl;
*/
		    ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 8;
		    break;
	    case 8: 
		    ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 9;
		    break;
	    case 9:
		    ROS_INFO("time is %d", t100ms_index); 
		    t100ms_index = 0;
		    break;
            defalut:
		    ROS_INFO("time is %d", t100ms_index); 
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


