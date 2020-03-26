#include <ros/ros.h>
#include <pigpiod_if2.h>
#include <thread>

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <unistd.h>

#include <bada_motor/RED.h>

const int MOTOR_DIR_R = 26;
const int MOTOR_PWM_R = 12;
const int MOTOR_ENA_R = 17;
const int MOTOR_ENB_R = 4;

const int MOTOR_DIR_L = 19;
const int MOTOR_PWM_L = 13;
const int MOTOR_ENA_L = 22; 
const int MOTOR_ENB_L = 27;


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

// NEW ADD

    int optMode = RED_MODE_DETENT;

    RED_t* Encoder_;

public:
    int pinum;
    int motor_ENA;
    int motor_ENB;
    int motor_DIR;
    int motor_PWM;
    int PWM_range;
    int PWM_frequency;
    int current_PWM;
    bool current_Direction;
    int acceleration;

    DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB, int Pi_Num);
    ~DCMotor();

    void EncoderPos();
    void MotorCtrl(bool Dir, int PWM);
    void AccelCtrl(bool Dir, int PWM_needed);

    //NEW ADD 
    static void cbf(int pos);

};

/************************************************************************
*                         Method of DCMotor Class                       *
************************************************************************/

DCMotor::DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB, int Pi_Num)
        : motor_DIR(M_Dir), motor_PWM(M_PWM), motor_ENA(M_encA), motor_ENB(M_encB), pinum_(Pi_Num){

    int optGpioA = -1;
    int optGpioB = -1;
    int optGlitch = 1000;
    int optSeconds = 0;

    if(pinum_ < 0){
      ROS_INFO("Setup failed");
      ROS_INFO("pinum is %d", pinum_);
    }

    PWM_range = 512;          
    PWM_frequency = 40000;    

    Encoder_ = RED(pinum_, motor_ENA, motor_ENB, optMode, cbf);
    RED_set_glitch_filter(Encoder_, optGlitch);  

    set_mode(Pi_Num, motor_DIR, PI_OUTPUT);
    set_mode(Pi_Num, motor_PWM, PI_OUTPUT);

    set_PWM_range(Pi_Num, motor_PWM, PWM_range);
    set_PWM_frequency(Pi_Num, motor_PWM, PWM_frequency);
    gpio_write(Pi_Num, motor_DIR, PI_LOW);
    set_PWM_dutycycle(Pi_Num, motor_PWM, 0);
    
    current_PWM = 0;
    current_Direction = true;
    acceleration = 5; // Acceleration

    ROS_INFO("Setup Fin");
}

DCMotor::~DCMotor(){
      RED_cancel(Encoder_);

}

void DCMotor::EncoderPos(){
    std::cout << RED_get_position(Encoder_) << std::endl;
}

void DCMotor::MotorCtrl(bool Dir, int PWM){
    if(Dir == true) { // CW
        gpio_write(pinum_, motor_DIR, PI_LOW);
        set_PWM_dutycycle(pinum_, motor_PWM, PWM);
        current_PWM = PWM;
        current_Direction = true;
    } else { // CCW
        gpio_write(pinum_, motor_DIR, PI_HIGH);
        set_PWM_dutycycle(pinum_, motor_PWM, PWM);
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

void DCMotor::cbf(int pos){
    std::cout << pos << std::endl;
}

//==================================DCMotor Class========================= 
//========================================================================
/////////////////////////////////////////////////////////////////////////////////////


int pinum = pigpio_start(NULL, NULL);
//pigpiod와 연결되면 0 이상의 값으로 리턴
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

int main (int argc, char **argv) {
    printf("Motor node Start \n");
    ros::init(argc, argv, "motor_node");
    ros::NodeHandle nh;

    Initialize();
	
    ros::Rate loop_rate(10);
 

    while(ros::ok()) {
	//Theta_Turn(90, 100);
      //loop_rate.sleep();
//	std::cout << "E1 : " << EncoderCounter1 << "    " << "E2 : " << EncoderCounter2 <<std::endl;
    }

    R_Motor.MotorCtrl(true, 0);
    L_Motor.MotorCtrl(true, 0);
    pigpio_stop(pinum);

    return 0;
}


