#include <ros/ros.h>
#include <pigpiod_if2.h>

const int MOTOR_DIR_R = 26;
const int MOTOR_PWM_R = 12;
const int MOTOR_ENA_R = 17;
const int MOTOR_ENB_R = 4;

const int MOTOR_DIR_L = 19;
const int MOTOR_PWM_L = 13;
const int MOTOR_ENA_L = 22; 
const int MOTOR_ENB_L = 27;


int PWM_Limit;

void Interrupt1(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt2(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

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

    DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB);
    ~DCMotor();
    void MotorCtrl(bool Dir, int PWM);
    void AccelCtrl(bool Dir, int PWM_needed);
};

/************************************************************************
*                         Method of DCMotor Class                       *
************************************************************************/

DCMotor::DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB)
        : motor_DIR(M_Dir), motor_PWM(M_PWM), motor_ENA(M_encA), motor_ENB(M_encB){
    pinum = pigpio_start(NULL, NULL); //pigpiod와 연결되면 0 이상의 값으로 리턴

    if(pinum < 0){
      ROS_INFO("Setup failed");
      ROS_INFO("pinum is %d", pinum);
    }

    //Setup PWM Duty Cycle and PWM frequency, Pin Mode
    PWM_range = 512;          
    PWM_frequency = 40000;    

    set_mode(pinum, motor_DIR, PI_OUTPUT);
    set_mode(pinum, motor_PWM, PI_OUTPUT);
    set_mode(pinum, motor_ENA, PI_INPUT);
    set_mode(pinum, motor_ENB, PI_INPUT);

    set_PWM_range(pinum, motor_PWM, PWM_range);
    set_PWM_frequency(pinum, motor_PWM, PWM_frequency);
    gpio_write(pinum, motor_DIR, PI_LOW);
    set_PWM_dutycycle(pinum, motor_PWM, 0);
    
    current_PWM = 0;
    current_Direction = true;
    acceleration = 5; // Acceleration

    ROS_INFO("Setup Fin");
}

DCMotor::~DCMotor(){
    pigpio_stop(pinum);
}

void DCMotor::MotorCtrl(bool Dir, int PWM){
    if(Dir == true) { // CW
        gpio_write(pinum, motor_DIR, PI_LOW);
        set_PWM_dutycycle(pinum, motor_PWM, PWM);
        current_PWM = PWM;
        current_Direction = true;
    } else { // CCW
        gpio_write(pinum, motor_DIR, PI_HIGH);
        set_PWM_dutycycle(pinum, motor_PWM, PWM);
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

//==================================DCMotor Class========================= 
//========================================================================
/////////////////////////////////////////////////////////////////////////////////////

bool motor1_ena_uprising;
bool motor1_ena_falling;
bool motor1_enb_uprising;
bool motor1_enb_falling;
bool motor2_ena_uprising;
bool motor2_ena_falling;
bool motor2_enb_uprising;
bool motor2_enb_falling;

DCMotor R_Motor = DCMotor(MOTOR_DIR_R, MOTOR_PWM_R, MOTOR_ENA_R, MOTOR_ENB_R);
DCMotor L_Motor = DCMotor(MOTOR_DIR_L, MOTOR_PWM_L, MOTOR_ENA_L, MOTOR_ENB_L);

//ENA 
//R_Motor
void Interrupt1_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
    int ENB = gpio_read(R_Motor.pinum, R_Motor.motor_ENB);
    if (ENB == PI_LOW)          EncoderCounter1 --;          //CCW
    else if (ENB == PI_HIGH)    EncoderCounter1 ++;          //CW
	std::cout << EncoderCounter1 << std::endl;
	std::cout << "Interrupt1_Falling" << std::endl;
}
void Interrupt1_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
    int ENB = gpio_read(R_Motor.pinum, R_Motor.motor_ENB);
    if (ENB == PI_LOW)          EncoderCounter1 ++;          //CW
    else if (ENB == PI_HIGH)    EncoderCounter1 --;          //CCW
	std::cout << "Interrupt1_R" << std::endl;
}

//L_Motor
void Interrupt2_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
    int ENB = gpio_read(L_Motor.pinum, L_Motor.motor_ENB);
    if (ENB == PI_LOW)          EncoderCounter2 --;           //CCW
    else if (ENB == PI_HIGH)    EncoderCounter2 ++;           //CW
	std::cout << "Interrupt2_F" << std::endl;
}
void Interrupt2_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
    int ENB = gpio_read(L_Motor.pinum, L_Motor.motor_ENB);
    if (ENB == PI_LOW)           EncoderCounter2 ++;          //CW
    else if (ENB == PI_HIGH)     EncoderCounter2 --;          //CCW
	std::cout << EncoderCounter2 << std::endl;
	std::cout << "Interrupt2_R" << std::endl;
}


//ENB
//R_Motor
void Interrupt3_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
    int ENA = gpio_read(R_Motor.pinum, R_Motor.motor_ENA);
    if (ENA == PI_LOW)           EncoderCounter1 ++;           //CW
    else if (ENA == PI_HIGH)     EncoderCounter1 --;           //CCW
	std::cout << "Interrupt3_F" << std::endl;
}
void Interrupt3_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
    int ENA = gpio_read(R_Motor.pinum, R_Motor.motor_ENA);
    if (ENA == PI_LOW)           EncoderCounter1 --;           //CCW
    else if (ENA == PI_HIGH)     EncoderCounter1 ++;           //CW 
	std::cout << "Interrupt3_R" << std::endl;
}

//L_Motor
void Interrupt4_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
    int ENA = gpio_read(L_Motor.pinum, L_Motor.motor_ENA);
    if (ENA == PI_LOW){            EncoderCounter2 ++;
	std::cout << "Interrupt4_F1" << std::endl;
}           //CW
    else if (ENA == PI_HIGH){      EncoderCounter2 --;           //CCW 
	std::cout << "Interrupt4_F2" << std::endl;
}
}
void Interrupt4_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick){
    int ENA = gpio_read(L_Motor.pinum, L_Motor.motor_ENA);
    if (ENA == PI_LOW){            EncoderCounter2 --;   
	std::cout << "Interrupt4_R1" << std::endl;}        //CCW
    else if (ENA == PI_HIGH){      EncoderCounter2 ++;           //CW
	std::cout << "Interrupt4_R2" << std::endl; 
}
}


void Initialize(){
    PWM_Limit = 150;

    motor1_ena_uprising = false;
    motor1_ena_falling  = false;
    motor1_enb_uprising = false;
    motor1_enb_falling  = false;

    motor2_ena_uprising = false;
    motor2_ena_falling  = false;
    motor2_enb_uprising = false;
    motor2_enb_falling  = false;

    EncoderCounter1 = 0;
    EncoderCounter2 = 0;
    set_pull_up_down(R_Motor.pinum, R_Motor.motor_ENA, PI_PUD_UP);
    set_pull_up_down(R_Motor.pinum, R_Motor.motor_ENB, PI_PUD_UP);
    set_pull_up_down(L_Motor.pinum, L_Motor.motor_ENA, PI_PUD_UP);
    set_pull_up_down(L_Motor.pinum, L_Motor.motor_ENB, PI_PUD_UP);

    ///callback(motor1.pinum, R_Motor.motor_ENA, FALLING_EDGE, Interrupt1);
    ///callback(motor1.pinum, L_Motor.motor_ENA, FALLING_EDGE, Interrupt2);

    ///////////////////////////////////////////////////////////////////////////////
    callback(R_Motor.pinum, R_Motor.motor_ENA, FALLING_EDGE, Interrupt1_Falling);
    callback(R_Motor.pinum, R_Motor.motor_ENA, RISING_EDGE,  Interrupt1_Rising);

    callback(L_Motor.pinum, L_Motor.motor_ENA, FALLING_EDGE, Interrupt2_Falling);
    callback(L_Motor.pinum, L_Motor.motor_ENA, RISING_EDGE,  Interrupt2_Rising);

    callback(R_Motor.pinum, R_Motor.motor_ENB, FALLING_EDGE, Interrupt3_Falling);
    callback(R_Motor.pinum, R_Motor.motor_ENB, RISING_EDGE,  Interrupt3_Rising);

    callback(L_Motor.pinum, L_Motor.motor_ENB, FALLING_EDGE, Interrupt4_Falling);
    callback(L_Motor.pinum, L_Motor.motor_ENB, RISING_EDGE,  Interrupt4_Rising);
    ///////////////////////////////////////////////////////////////////////////////

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
        ros::spinOnce();
        //loop_rate.sleep();

    }

    R_Motor.MotorCtrl(true, 0);
    L_Motor.MotorCtrl(true, 0);

    return 0;
}


