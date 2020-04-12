// BADA_CTRL_H_
#ifndef BADA_CTRL_H_  
#define BADA_CTRL_H_
 
#include <bada_ctrl/RED.h>
#include <pigpiod_if2.h>
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
const int ENCODER_RESOLUTION = 374; 	    // count per cycle

const double PI = 3.141592;

//----------------------------------------------------------------------------------
/************************************************************************
*                            DCMotor Class                              *
************************************************************************/

class DCMotor{
private:
    MotorPosition MotorPosition_;
    static void cbf_(int pos);
    RED_t* Encoder_;  //Encoder Setting    

    int nPin_;

    int MTR_ENA_;
    int MTR_ENB_;
    int MTR_DIR_;
    int MTR_PWM_;

    int PWM_range_;
    int PWM_frequency_;
    int PWM_currunt_;
    int pinum_;
    
    //
    int optGlitch_;
    int optMode_;
    
    //For PID
    float P_gain_;
    float I_gain_;
    float D_gain_;
    
    float err_k_1_;
    float err_sum_;

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

    void PIDgainSET(float P_gain, float I_gain, float D_gain );

    int EncoderPos();
    void MotorCtrl(bool Dir, int PWM);
    void AccelCtrl(bool Dir, int PWM_needed);
    static void printEncoderPos(int pos);

    void SetVel_Target(double Vel);
    void PIDCtrl_(float TargetSpd);

    double VelEnc_Transform_(double vel);
    double EncVel_Transform_(int diff_Enc);

    void EncoderDiff();
    void CalVel();
    void PIDUpdate();
};

#endif // BADA_H_

