#include <bada_ctrl/bada_ctrl.h>

/************************************************************************
*                         Method of DCMotor Class                       *
************************************************************************/

DCMotor::DCMotor(int M_Dir, int M_PWM, int M_encA, int M_encB, int Pi_Num, MotorPosition MPosition)
        : MTR_DIR_(M_Dir), MTR_PWM_(M_PWM), MTR_ENA_(M_encA), MTR_ENB_(M_encB), pinum_(Pi_Num), MotorPosition_(MPosition) 
{

    optGlitch_ = 100;
    optMode_ = RED_MODE_STEP;
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
    temp_RPM = double(diff_Enc) /20 * 60 / 1 * 1000 / 1 * 1 / ENCODER_RESOLUTION; 
    return temp_RPM / 1 * 1 / 60 * 2*WHEELSIZE*PI / 1 * 1 / 1000; // Velocity
} //Control Freq == 20ms

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
void DCMotor::cbf_(int pos){
    std::cout << pos << std::endl;
}
void DCMotor::EncoderDiff(){
    Pos_now  = EncoderPos();
    Pos_diff = Pos_now - Pos_prev;	
    if (MotorPosition_ == RIGHT)
	Pos_diff =-Pos_diff;
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

    long double TargetENC;
    long double ENC;
    long double input_spd;

    TargetENC = VelEnc_Transform_(TargetSpd);
    ENC       = VelEnc_Transform_(Spd);

    err = TargetENC - ENC;
    err_sum_ += err;
    
    uP = P_gain_ * err;
    uI = I_gain_ * err_sum_;
    uD = D_gain_ * (err - err_k_1_);
    	
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
	        m_dir = false ;
		    input_u *= -1;
	    }
	    else {
	        m_dir = true;
	    }
    }

    if (input_u > 512) u_val = 512;
    else u_val = input_u; input_spd = EncVel_Transform_(u_val);
    
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


