#include <mobile_robot/motor_algorithm.h>
    

DcMotorForRaspberryPi motor1 = DcMotorForRaspberryPi(motor_DIR1, motor_PWM1, motor_ENA1, motor_ENB1);
DcMotorForRaspberryPi motor2 = DcMotorForRaspberryPi(motor_DIR2, motor_PWM2, motor_ENA2, motor_ENB2);

using namespace std;
int PWM_limit;

void Interrupt1_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt1_Uprising(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

void Interrupt2_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt2_Uprising(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

void Interrupt3_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt3_Uprising(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

void Interrupt4_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick);
void Interrupt4_Uprising(int pi, unsigned user_gpio, unsigned level, uint32_t tick);

volatile int EncoderCounter1;
volatile int EncoderCounter2;

bool motor1_ena_uprising;
bool motor1_ena_falling;

bool motor1_enb_uprising;
bool motor1_enb_falling;

bool motor2_ena_uprising;
bool motor2_ena_falling;

bool motor2_enb_uprising;
bool motor2_enb_falling;

bool switch_direction;
int Theta_Distance_Flag;

/////////////////////////////////////////////////////////////////////////////////////
void motor_first_command_callback(const mobile_robot::motor_cmd::ConstPtr& msg)
{
  // motor1->speed_motor = msg->motor_desired_speed;
  // motor1->onoff       = msg->motor_onoff;
  // motor1.Motor_Controller(true, local_PWM);
  //motor1.Motor_Controller(true,msg->motor_desired_speed);
  motor1.Accel_Controller(msg->motor_desired_direction,msg->motor_desired_speed);
  //printf("motor1_speed : %f\n", msg->motor_desired_speed);
  //printf("motor1_dir : %d\n", msg->motor_desired_direction);
}
void motor_second_command_callback(const mobile_robot::motor_cmd::ConstPtr& msg)
{
  // motor2->speed_motor = msg->motor_desired_speed;
  // motor2->onoff       = msg->motor_onoff;
  // motor2.Motor_Controller(true, local_PWM);
  //motor2.Motor_Controller(true,msg->motor_desired_speed);
  motor2.Accel_Controller(msg->motor_desired_direction,msg->motor_desired_speed);
  //printf("motor2_speed : %f\n", msg->motor_desired_speed);
  //printf("motor2_dir : %d\n", msg->motor_desired_direction);
}
void left_motor_cmd_callback(const mobile_robot::motor_cmd::ConstPtr& msg)
{
  motor1.Accel_Controller(msg->motor_desired_direction, msg->motor_desired_speed);
}
void right_motor_cmd_callback(const mobile_robot::motor_cmd::ConstPtr& msg)
{
  motor2.Accel_Controller(msg->motor_desired_direction, msg->motor_desired_speed);
}
/////////////////////////////////////////////////////////////////////////////////////
void Initialize()
{
  PWM_limit = 150;
  EncoderCounter1 = 0;
  EncoderCounter2 = 0;

  bool motor1_ena_uprising = false;
  bool motor1_ena_falling = false;

  bool motor1_enb_uprising = false;
  bool motor1_enb_falling = false;

  bool motor2_ena_uprising = false;
  bool motor2_ena_falling = false;

  bool motor2_enb_uprising = false;
  bool motor2_enb_falling = false;

  callback(motor1.pinum, motor1.motor_ENA, FALLING_EDGE, Interrupt1_Falling);
  callback(motor1.pinum, motor1.motor_ENA, RISING_EDGE, Interrupt1_Rising);

  callback(motor2.pinum, motor2.motor_ENA, FALLING_EDGE, Interrupt2_Falling);
  callback(motor2.pinum, motor2.motor_ENA, RISING_EDGE, Interrupt2_Rising);
  
  callback(motor2.pinum, motor2.motor_ENB, FALLING_EDGE, Interrupt2_Falling);
  callback(motor2.pinum, motor2.motor_ENB, RISING_EDGE, Interrupt2_Rising);
  
  callback(motor2.pinum, motor2.motor_ENB, FALLING_EDGE, Interrupt2_Falling);
  callback(motor2.pinum, motor2.motor_ENB, RISING_EDGE, Interrupt2_Rising);
  
  switch_direction = true;
  Theta_Distance_Flag = 0;

  ROS_INFO("Initialize Complete");
}

void Interrupt1_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
  motor1_ena_falling = !motor1_ena_falling;

  EncoderCounter1 ++;
  //ROS_INFO("Interrupt1 is %d", EncoderCounter1);
}

void Interrupt1_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
  motor1_ena_uprising = !motor1_ena_uprising;

  EncoderCounter1 ++;
  //ROS_INFO("Interrupt2 is %d", EncoderCounter2);
}

void Interrupt2_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
  motor2_ena_falling = !motor2_ena_falling;

  EncoderCounter2 ++;
  //ROS_INFO("Interrupt1 is %d", EncoderCounter1);
}

void Interrupt2_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
  motor2_ena_uprising = !motor2_ena_uprising;

  EncoderCounter2 ++;
  //ROS_INFO("Interrupt2 is %d", EncoderCounter2);
}

void Interrupt3_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
  motor1_enb_falling = !motor1_enb_falling;

  EncoderCounter1 ++;
  //ROS_INFO("Interrupt1 is %d", EncoderCounter1);
}

void Interrupt3_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
  motor1_enb_uprising = !motor1_enb_uprising;

  EncoderCounter1 ++;
  //ROS_INFO("Interrupt2 is %d", EncoderCounter2);
}

void Interrupt4_Falling(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
  motor2_enb_falling = !motor2_enb_falling;

  EncoderCounter2 ++;
  //ROS_INFO("Interrupt1 is %d", EncoderCounter1);
}

void Interrupt4_Rising(int pi, unsigned user_gpio, unsigned level, uint32_t tick)
{
  motor2_enb_uprising = !motor2_enb_uprising;

  EncoderCounter2 ++;
  //ROS_INFO("Interrupt2 is %d", EncoderCounter2);
}


int Limit_Function(int pwm)
{
  int output;
  if(pwm > PWM_limit)output = PWM_limit;
  else if(pwm < 0)output = 0;
  else output = pwm;
  return output; 
}


/////////////////////////////////////////////////////////////////////////////////////
DcMotorForRaspberryPi::DcMotorForRaspberryPi(int motor_dir, int motor_pwm, int motor_ena, int motor_enb)
{
  pinum=pigpio_start(NULL, NULL);
  if(pinum<0)
  {
    ROS_INFO("Setup failed");
    ROS_INFO("pinum is %d", pinum);
  }
  motor_DIR = motor_dir;
  motor_PWM = motor_pwm;
  motor_ENA = motor_ena;
  PWM_range = 512;
  PWM_frequency = 40000; 

  set_mode(pinum, motor_dir, PI_OUTPUT);
  //ROS_INFO("pinum is %d", pinum);
  //ROS_INFO("motor_dir is %d", motor_dir);
  set_mode(pinum, motor_pwm, PI_OUTPUT);
  set_mode(pinum, motor_ena, PI_INPUT);

  set_PWM_range(pinum, motor_pwm, PWM_range);
  set_PWM_frequency(pinum, motor_pwm, PWM_frequency);
  gpio_write(pinum, motor_DIR, PI_LOW);
  //ROS_INFO("pinum is %d", pinum);
  //ROS_INFO("motor_dir is %d", motor_dir);
  set_PWM_dutycycle(pinum, motor_PWM, 0);
  
  current_PWM = 0;
  current_Direction = true;
  acceleration = 5;
  ROS_INFO("Setup Fin");
}
void DcMotorForRaspberryPi::Motor_Controller(int direction, int pwm)
{
  int local_PWM = Limit_Function(pwm);
  if(direction == 1) //CW
  {
    gpio_write(pinum, motor_DIR, PI_LOW);
    set_PWM_dutycycle(pinum, motor_PWM, local_PWM);
    current_PWM = local_PWM;
    current_Direction = true;
  }
  else if(direction == -1) //CCW
  {
    gpio_write(pinum, motor_DIR, PI_HIGH);
    set_PWM_dutycycle(pinum, motor_PWM, local_PWM);
    current_PWM = local_PWM;
    current_Direction = false;
  }
}

void DcMotorForRaspberryPi::Accel_Controller(int direction, int desired_pwm)
{
  int local_PWM;
  if(desired_pwm > current_PWM)
  {
    local_PWM = current_PWM + acceleration;
    Motor_Controller(direction, local_PWM);
  }
  else if(desired_pwm < current_PWM)
  {
    local_PWM = current_PWM - acceleration;
    Motor_Controller(direction, local_PWM);
  }
  else
  {
    local_PWM = current_PWM;
    Motor_Controller(direction, local_PWM);
  }
  //ROS_INFO("Current_PWM is %d", current_PWM);
}

void DcMotorForRaspberryPi::speed_controller(int desired_speed)
{
  speed_static_encoder_pulse_ = (encoder_pulse1*2); // digital low pass filter  // basic 2 ch

   //speed_static_encoder_pulse_ = (encoder_pulse1+ encoder_pulse2);
  encoder_pulse1 = 0;
  encoder_pulse2 = 0;
  result_rpm =  (((speed_static_encoder_pulse_)*60*control_freqency_)/(encoder_pulse_per_rotation_*channel_));


  speed_error_ = desired_speed  -  result_rpm ;
  speed_control_ = ( p_gain_speed_ * speed_error_);

  pwm_value_motor = (pwm_value_motor + speed_control_);

  if (pwm_value_motor > 512)
  {
    pwm_value_motor = 512;
  }

  if (pwm_value_motor < 0)
  {
    pwm_value_motor = 0;
  }

}

double DcMotorForRaspberryPi::position_controller(int desired_angle, int max_rpm)
{
  position_static_encoder_pulse_ = (encoder_pulse_position1+ encoder_pulse_position2);

  if(((desired_angle*encoder_pulse_per_rotation_*channel_)/360) <= position_static_encoder_pulse_)
  {
    check_position = true;
    return 0;
  }
  else
  {
    position_error_ = ((desired_angle*encoder_pulse_per_rotation_*channel_)*360) - position_static_encoder_pulse_;
    position_control_ = p_gain_position_ * position_error_;

    if(position_control_ >  max_rpm)
    {
      position_control_ = max_rpm;
    }
    if(position_control_ < 0)
    {
      position_control_ = 0;
    }
    return position_control_;
  }

}


/////////////////////////////////////////////////////////////////////////////////////

int main (int argc, char **argv)
{
  printf("Motor node Start \n");
  ros::init(argc, argv, "motor_node");
  ros::NodeHandle nh;

   
  Initialize();
  ros::Rate loop_rate(10);

  // std::string motor_first_topic = nh.param<std::string>("motor_first", "");
  // std::string motor_second_topic = nh.param<std::string>("motor_second", "");
  // motor_first_command_sub   = nh.subscribe(motor_first_topic, 1, motor_first_command_callback);
  // motor_second_command_sub  = nh.subscribe(motor_second_topic, 1, motor_second_command_callback);
  
  ros::Subscriber motor_first_command_sub  = nh.subscribe("/motor_1", 10, motor_first_command_callback);
  ros::Subscriber motor_second_command_sub = nh.subscribe("/motor_2", 10, motor_second_command_callback);
  ros::Subscriber left_motor_cmd_sub = nh.subscribe("/heroehs/ssoni_robot/left_motor_cmd", 10, left_motor_cmd_callback);
  ros::Subscriber right_motor_cmd_sub = nh.subscribe("/heroehs/ssoni_robot/right_motor_cmd", 10, right_motor_cmd_callback);

  // while(ros::ok())
  // {
  //   usleep(100);
  //   ros::spinOnce();
  // }
  while(ros::ok())
  {
    ros::spinOnce();
    loop_rate.sleep();
  }

  motor1.Motor_Controller(true, 0);
  motor2.Motor_Controller(true, 0);

  return 0;
}
