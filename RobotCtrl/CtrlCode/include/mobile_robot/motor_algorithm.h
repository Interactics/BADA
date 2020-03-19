//pin information
#define motor_DIR1 26
#define motor_PWM1 12
#define motor_ENA1 6
#define motor_ENB1 5

#define motor_DIR2 19
#define motor_PWM2 13
#define motor_ENA2 22 
#define motor_ENB2 27

#include <ros/ros.h>
#include <stdio.h>
#include <math.h>
#include <pigpiod_if2.h>

//ros_communication_message type
#include <std_msgs/String.h>
#include <std_msgs/Float64.h>
#include <std_msgs/Float64MultiArray.h>
#include <geometry_msgs/Twist.h>

//custom header
#include <mobile_robot/motor_cmd.h>

class DcMotorForRaspberryPi
{
  public:

    DcMotorForRaspberryPi(int motor_dir, int motor_pwm, int motor_ena, int motor_enb);
    void Motor_Controller(int direction, int pwm);
    void Accel_Controller(int direction, int desired_pwm);

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

    double pwm_value_motor;
    double speed_motor;
    double angle_motor;
    double result_rpm;
    bool direction;
    bool check_position_control;
    bool onoff;

    int encoder_pulse1;
    int encoder_pulse2;

    int encoder_pulse_position1;
    int encoder_pulse_position2;

    int position_max_rpm;
    bool check_position;

    double acceleration_value;

    void speed_controller(int desired_speed);
    double position_controller(int desired_angle, int max_rpm);

  private:
    int encoder_pulse_per_rotation_;
    int control_freqency_;
    int channel_;

    double p_gain_position_;
    double p_gain_speed_;

    double speed_static_encoder_pulse_;
    double speed_error_;
    double speed_control_;

    double position_static_encoder_pulse_;
    double position_error_;
    double position_control_;
};

//ros communication

ros::Subscriber motor_first_command_sub;
ros::Subscriber motor_second_command_sub;
ros::Subscriber left_motor_cmd_sub;
ros::Subscriber right_motor_cmd_sub;

//function
void Initialize();

void Interrupt1_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt1_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

void Interrupt2_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt2_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

void Interrupt3_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt3_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

void Interrupt4_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt4_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick);


int Limit_Function(int pwm);

void motor_first_command_callback(const mobile_robot::motor_cmd::ConstPtr& msg);
void motor_second_command_callback(const mobile_robot::motor_cmd::ConstPtr& msg);
void left_motor_cmd_callback(const mobile_robot::motor_cmd::ConstPtr& msg);
void right_motor_cmd_callback(const mobile_robot::motor_cmd::ConstPtr& msg);  
