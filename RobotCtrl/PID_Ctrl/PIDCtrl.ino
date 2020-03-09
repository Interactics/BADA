#include <TimerFive.h>
#include <ros.h>
#include <std_msgs/Float32.h>

unsigned long time;

//=============================
//motor gain
//-----------------------------
//motor A > right
#define MA_KP  0.62  //0.6
#define MA_KD  0.4  //0.4
#define MA_KI  0.13  //0.1

//motor B > left
#define MB_KP  0.61
#define MB_KD  0.45
#define MB_KI  0.14
//=============================

#define EC_CHA 3
#define EC_CHB 49
#define MC_DIR1 42
#define MC_DIR2 4
#define MC_PWM 9

#define EB_CHA 19
#define EB_CHB 38
#define MB_DIR1 36
#define MB_DIR2 37
#define MB_PWM 8

#define EA_CHA 18
#define EA_CHB 31
#define MA_DIR1 35
#define MA_DIR2 34
#define MA_PWM 12

#define go 0
#define back 1

#define CCW 1
#define CW 0

//Robot Setting
#define Wheel_D 13

long  encoder_a = 0;
long  encoder_a_k_1 = 0;
long  encoder_b = 0;
long  encoder_b_k_1 = 0;

int speed_a = 0;
int speed_b = 0;

int speed_ref_a = 0;
int speed_ref_b = 0;

float avg_wheel_speed = 0;

float get_speed = 0;

//for Timer5
//------------------------
bool t5_flag = 0;
unsigned int  t5_index = 0;
unsigned int  send_t_cnt = 0;
//-------------------------

/*
  void motor_move(float target_speed);  // input : val of (m/s), + front , - backward.
  void motor_all(bool DIR_a = CW, int a_speed = 0, bool DIR_b = CW,
               int b_speed = 0, bool DIR_c = CW, int c_speed = 0); // controlling each motor's speed ,spd is PWM val
  void motor_turn(char motor_name, bool DIR, int motor_sp); // motor_sp : PWM
*/

//Pub
ros::NodeHandle nh;
std_msgs::Float32 msg;
ros::Publisher Speed_pub("m_speed", &msg);

//Sub
void messageCb( const std_msgs::Float32& speed_pub) {
  get_speed = speed_pub.data;
}      // callback Function.

ros::Subscriber <std_msgs::Float32> sub("get_speed", messageCb);


void setup() {
  pinMode(22, OUTPUT);
  pinMode(EA_CHA, INPUT);
  pinMode(EA_CHB, INPUT);
  pinMode(MA_DIR1, OUTPUT);
  pinMode(MA_DIR2, OUTPUT);
  pinMode(MA_PWM, OUTPUT);

  pinMode(EB_CHA, INPUT);
  pinMode(EB_CHB, INPUT);
  pinMode(MB_DIR1, OUTPUT);
  pinMode(MB_DIR2, OUTPUT);
  pinMode(MB_PWM, OUTPUT);

  pinMode(EC_CHA, INPUT);
  pinMode(EC_CHB, INPUT);
  pinMode(MC_DIR1, OUTPUT);
  pinMode(MC_DIR2, OUTPUT);
  pinMode(MC_PWM, OUTPUT);

  /**/
    nh.initNode();
    nh.advertise(Speed_pub);
    nh.subscribe(sub);
  
  //Serial.begin(9600);

  Timer5.initialize(10000);  //10msec
  Timer5.attachInterrupt(T5_ISR);

  attachInterrupt(digitalPinToInterrupt(EA_CHA), Encoder_A, CHANGE);
  attachInterrupt(digitalPinToInterrupt(EB_CHA), Encoder_B, CHANGE);

  t5_flag = 0;
}

void loop() {
  if (t5_flag) {
    t5_flag = 0;
    switch (t5_index) {
      case 0:
        t5_index = 1;
        nh.spinOnce();
        //get_speed = 0.5;
        break;

      case 1:
        t5_index = 2;
        // 0.13(wheel_dia)*pi/663 [m/pulse]
        // [m/s] -> pulse/50msec => 0.05 * (663/0.13*pi) = 81.169
        speed_ref_a = get_speed * 81.169;
        speed_ref_b = get_speed * (-81.145);

        //present wheel speed(50msec)
        speed_a = encoder_a - encoder_a_k_1;
        speed_b = encoder_b - encoder_b_k_1;
        encoder_a_k_1 = encoder_a;
        encoder_b_k_1 = encoder_b;
        break;

      case 2:
        t5_index = 3;
        Motor_A_Speed_Ctrl();
        break;

      case 3:
        t5_index = 4;
        Motor_B_Speed_Ctrl();
        break;

      case 4:
        t5_index = 5;
        // wheel speed to robot speed value
        avg_wheel_speed = speed_a;
        avg_wheel_speed /= 81.169;
        msg.data = avg_wheel_speed;
        break;

      case 5:
        t5_index = 6;
        // send the present speed
        send_t_cnt++;
        if(send_t_cnt>10){
          send_t_cnt = 0;
          Speed_pub.publish(&msg);
        }
        break;

      case 6:
        t5_index = 7;
        //present wheel speed(50msec)
        speed_a = encoder_a - encoder_a_k_1;
        speed_b = encoder_b - encoder_b_k_1;
        encoder_a_k_1 = encoder_a;
        encoder_b_k_1 = encoder_b;
        break;

      case 7:
        t5_index = 8;
        Motor_A_Speed_Ctrl();
        break;

      case 8:
        t5_index = 9;
        Motor_B_Speed_Ctrl();
        break;

      case 9:
        t5_index = 0;
        break;
      default:
        t5_index = 0;
        break;
    } //end of 'switch'
  }//end of 'if'

}//end of 'loop func'

void Motor_A_Speed_Ctrl() {
  float err = 0;
  float up = 0, ui = 0, ud = 0;
  float input_u = 0;
  int   u_val = 0;
  bool  m_dir = 0;
  static float  err_k_1 = 0;
  static float  err_sum = 0;

  err = speed_ref_a - speed_a;
  err_sum += err;

  up = MA_KP * err;
  ui = MA_KI * err_sum;
  ud = MA_KD * (err - err_k_1);

  err_k_1 = err;
  input_u = up + ui + ud;

  if (input_u < 0) {
    m_dir = 1;
    input_u *= -1;
  }
  else {
    m_dir = 0;
  }

  if (input_u > 255) u_val = 255;
  else u_val = input_u;

  digitalWrite(MA_DIR1, m_dir);
  digitalWrite(MA_DIR2, !m_dir);
  analogWrite(MA_PWM, u_val);
}

void Motor_B_Speed_Ctrl() {
  float err = 0;
  float up = 0, ui = 0, ud = 0;
  float input_u = 0;
  int   u_val = 0;
  bool  m_dir = 0;
  static float  err_k_1 = 0;
  static float  err_sum = 0;

  err = speed_ref_b - speed_b;
  err_sum += err;

  up = MB_KP * err;
  ui = MB_KI * err_sum;
  ud = MB_KD * (err - err_k_1);

  err_k_1 = err;
  input_u = up + ui + ud;

  if (input_u < 0) {
    m_dir = 1;
    input_u *= -1;
  }
  else {
    m_dir = 0;
  }
  if (input_u > 255) u_val = 255;
  else u_val = input_u;

  digitalWrite(MB_DIR1, m_dir);
  digitalWrite(MB_DIR2, !m_dir);
  analogWrite(MB_PWM, u_val);
}

void Encoder_A() {
  if (digitalRead(EA_CHA) == digitalRead(EA_CHB))
    encoder_a++;
  else
    encoder_a--;
}

void Encoder_B() {
  if (digitalRead(EB_CHA) == digitalRead(EB_CHB))
    encoder_b++;
  else
    encoder_b--;
}
/*
  void Encoder_C() {
  if (digitalRead(EC_CHA) == digitalRead(EC_CHB))
    encoder_c++;
  else
    encoder_c--;
  }
*/
void T5_ISR() {
  t5_flag = 1;
  //t5_index++;
}
