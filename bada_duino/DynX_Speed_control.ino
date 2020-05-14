#include "DynamixelMotor.h"                                 // Dynamixel
#include <RGBmatrixPanel.h>                                 // RGBMatrixPanel

#define CLK 11
#define OE   9
#define LAT 10
#define A   A0
#define B   A1
#define C   A2
#define D   A3                                              ///Setting For Display
#define D_ID 144                                            ///Dynmixel ID

const long unsigned int baudrate = 1000000;                 // communication baudrate
const long timeInterval = 50;
int16_t speed = 512;                                        // speed, between 0 and 1023
int ledState1 = LOW;

//for Timer5
//------------------------
bool t5_flag = false;
unsigned int  t5_index = 0;
unsigned int  send_t_cnt = 0;
//-------------------------

void Dynamixle_setting();
float Trajectory(float t);
float t_val = 0;

HardwareDynamixelInterface interface(Serial1);              // Serial1 -- RX2 TX2, Serial -- RX1 TX1
DynamixelMotor motor(interface, D_ID);
RGBmatrixPanel matrix(A, B, C, D, CLK, LAT, OE, false);

unsigned volatile long previousTime = millis();
unsigned volatile long currentTime;

void setup() {
  // check communication with dynamixel
  interface.begin(baudrate);
  delay(100);

  Dynamixle_setting();
  t5_flag = false;
  
  matrix.begin();
}

void loop()
{
  currentTime = millis();
  if(currentTime - previousTime > timeInterval) {
    previousTime = currentTime;
    t5_flag = true;
  }
  
  // go to middle position
//  motor.goalPosition(650);
//  delay(500);
//  matrix.fillRect(0, 0, 32, 32, matrix.Color333(7, 7, 0));
//  
//  // move 45° CCW
//  motor.goalPosition(666);
//  delay(500);
//  matrix.fillRect(0, 0, 32, 32, matrix.Color333(0, 7, 0));
//
//  // go to middle position
//  motor.goalPosition(660);
//  matrix.fillRect(0, 0, 32, 32, matrix.Color333(7, 7, 7));
//  delay(500);
  
  if (t5_flag) {
    t5_flag = false;

    switch (t5_index) {
      case 0:
        t5_index = 1;
            motor.goalPosition(Trajectory(t_val));
        break;
      case 1:
        t5_index = 2;

        
          motor.goalPosition(Trajectory(t_val));
          
        break;
      case 2:
        t5_index = 3;
                  motor.goalPosition(Trajectory(t_val));
        break;
      case 3:
        t5_index = 4;
          motor.goalPosition(Trajectory(t_val));
        break;
      case 4:
        t5_index = 5;
          motor.goalPosition(Trajectory(t_val));
        break;
      case 5:
        t5_index = 6;
          motor.goalPosition(Trajectory(t_val));
        break;
      case 6:
        t5_index = 7;
          motor.goalPosition(Trajectory(t_val));
        break;
      case 7:
        t5_index = 8;
          motor.goalPosition(Trajectory(t_val));
        break;
      case 8:
        t5_index = 9;
          motor.goalPosition(Trajectory(t_val));
        break;
      case 9:
        t5_index = 0;
          motor.goalPosition(Trajectory(t_val));
        break;
      default:
        t5_index = 0;
        
        break;
    } //end of 'switch'
    if(t_val < 3) t_val = t_val + 0.05;
  }//end of 'if'
}//end of 'loop func'


void Dynamixle_setting() {
  motor.enableTorque();  // set to joint mode, with a 180° angle range
  motor.jointMode(204, 820);  // see robotis doc to compute angle values
  motor.speed(speed);

  uint8_t status = motor.init();
  if (status != DYN_STATUS_OK) {
    // Fail to connect with dynamixel
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    while (1);
  }
}

float Trajectory(float t){
  return (-133.3/3* t*t*t + 200 *t*t + 200);
}
