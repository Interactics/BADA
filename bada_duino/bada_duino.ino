//bada_duino.ino
//This .ino is made for controlling Dot Matrix, Dynamixel AX-12 and Tactswitch.


#include <TimerFive.h>
#include <ros.h>
#include <std_msgs/Float32.h>
#include "DynamixelMotor.h"

ros::NodeHandle nh;
std_msgs::Float32 msg;
ros::Publisher Speed_pub("m_speed", &msg);

void messageCb(const std_msgs::Float32 &speed_pub){
    get_speed = speed_pub.data;
} // callback Function.

unsigned long time;

// Test motor joint mode

// id of the motor
const uint8_t id = 1;
// speed, between 0 and 1023
int16_t speed = 512;
// communication baudrate
const long unsigned int baudrate = 1000000;
// hardware serial without tristate buffer
// see blink_led example, and adapt to your configuration
HardwareDynamixelInterface interface(Serial);
DynamixelMotor motor(interface, id);

void setup(){

    interface.begin(baudrate);
    delay(100);
    // check if we can communicate with the motor
    // if not, we turn the led on and stop here
    uint8_t status = motor.init();
    if (status != DYN_STATUS_OK)
    {
        pinMode(LED_BUILTIN, OUTPUT);
        digitalWrite(LED_BUILTIN, HIGH);
        while (1)
            ;
    }
    motor.enableTorque();
    // set to joint mode, with a 180° angle range
    // see robotis doc to compute angle values
    motor.jointMode(204, 820);
    motor.speed(speed);
    Timer5.initialize(10000); //10msec
    Timer5.attachInterrupt(T5_ISR);
}

void loop(){
    // go to middle position
    motor.goalPosition(512);
    delay(500);

    // move 45° CCW
    motor.goalPosition(666);
    delay(500);

    // go to middle position
    motor.goalPosition(512);
    delay(500);

    // move 45° CW
    motor.goalPosition(358);
    delay(500);
}

void loop(){
    if (t5_flag)    {
        t5_flag = 0;
        switch (t5_index)
        { // 10ms
        case 0:
            t5_index = 1;
            break;

        case 1:
            t5_index = 2;
            break;

        case 2:
            t5_index = 3;
            break;

        case 3:
            t5_index = 4;
            break;

        case 4:
            t5_index = 5;
            break;

        case 5:
            t5_index = 6;
            break;

        case 6:
            t5_index = 7;
            break;

        case 7:
            t5_index = 8;
            break;

        case 8:
            t5_index = 9;
            break;

        case 9:
            t5_index = 0;
            break;
        default:
            t5_index = 0;
            break;
        } //END switch
    }     //END if
} //END loop