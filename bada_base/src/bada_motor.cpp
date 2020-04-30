#include <bada_ctrl/bada_ctrl.h>
#include <ros/ros.h>
#include <pigpiod_if2.h>
#include <geometry_msgs/Twist.h>
#include <tf/transform_broadcaster.h>
#include <nav_msgs/Odometry.h>
#include <cmath>
#include <std_msgs/Float64.h>

void Initialize();
void TwistToMotor_CB(const geometry_msgs::Twist &msg);
void timer_CB(const ros::TimerEvent &event);
void MotorVelControling();
void PubMotRLSpd(ros::Publisher *R, ros::Publisher *L);

double Vel_R = 0,  Vel_L = 0; 			         // [m/s]
double aVel_R = 0, aVel_L = 0; 			         // anguler velocities [Rad/sec]
double Linear_Vel = 0, Angular_Vel = 0;    		 // [m/s], [Rad/sec]
double x_dist, y_dist = 0;
double Angle_delta = 0, Angle = 0;
double Distance_delta = 0;

// TIME Interrupt
volatile bool t100ms_flag = false;
int t100ms_index = 0;

// if it were connected with pigpiod, 0 value is returend.
int pinum = pigpio_start(NULL, NULL);

// Motor Obj
DCMotor R_Motor = DCMotor(MOTOR_DIR_R, MOTOR_PWM_R, MOTOR_ENA_R, MOTOR_ENB_R, pinum, RIGHT);
DCMotor L_Motor = DCMotor(MOTOR_DIR_L, MOTOR_PWM_L, MOTOR_ENA_L, MOTOR_ENB_L, pinum, LEFT) ;

int main (int argc, char **argv) {
    ros::init(argc, argv, "bada");
    ros::NodeHandle nh;
    Initialize();
    
    nav_msgs::Odometry odom;
    geometry_msgs::Twist cmd_vel;
    geometry_msgs::Quaternion odom_quat;
    geometry_msgs::TransformStamped odom_trans;

    tf::TransformBroadcaster odom_broadcaster;
    ros::Time current_time;
    ros::Time begin = ros::Time::now();
    ros::Timer timer = nh.createTimer(ros::Duration(0.01), timer_CB);

    //Odometry Inform
    ros::Publisher bada_odom = nh.advertise<nav_msgs::Odometry>("/bada/odom", 1);
    ros::Subscriber sub_vel = nh.subscribe("cmd_vel", 1, TwistToMotor_CB);
    ros::Rate loop_rate(10);

    ros::Publisher bada_MotorR = nh.advertise<std_msgs::Float64>("/bada/motorR",1);
    ros::Publisher bada_MotorL = nh.advertise<std_msgs::Float64>("/bada/motorL",1);

    while(ros::ok()) {
        ros::spinOnce();
        if(t100ms_flag){
            t100ms_flag = false;
            switch(t100ms_index){
                case 0: 
                    MotorVelControling();
                   
	            t100ms_index = 1;
	            break;
		case 1: 
		    t100ms_index = 2;
		    break;
		case 2: 
                    MotorVelControling();
                   
		    t100ms_index = 3;
		    break;
		case 3: 
		    t100ms_index = 4;
		    break;
		case 4:  
                    MotorVelControling();

		    Vel_R = R_Motor.VELOCITY;
		    Vel_L = L_Motor.VELOCITY;
	            aVel_R = Vel_R / (2 * WHEELSIZE);
		    aVel_L = Vel_L / (2 * WHEELSIZE);

		    Linear_Vel  = (Vel_R + Vel_L) / 2;
		    Angular_Vel = (Vel_R - Vel_L) / WHEELBASE * 1000 ;
				
//		    ROS_INFO("Linear : %.2f m/s\tAng : %.2f rad", Linear_Vel, Angular_Vel);
//		    ROS_INFO("Vel_L : %.2f m/s \tVel_R : %.2f m/s", Vel_L, Vel_R);
	
		    t100ms_index = 5;
		    break;
	        case 5: //Publish Part
		    Vel_R = R_Motor.VELOCITY;
		    Vel_L = L_Motor.VELOCITY;
	            aVel_R = Vel_R / (2 * WHEELSIZE);
		    aVel_L = Vel_L / (2 * WHEELSIZE);

		    Linear_Vel  = (Vel_R + Vel_L) / 2;
		    Angular_Vel = (Vel_R - Vel_L) / WHEELBASE * 1000 ;
	
	            cmd_vel.linear.x = Linear_Vel;
		    cmd_vel.angular.z = Angular_Vel;
		    t100ms_index = 6;
		    break;
		case 6:
                    MotorVelControling();
		    
		    Distance_delta = Linear_Vel*0.1;
		    Angle_delta = Angular_Vel*0.1;
		    x_dist = x_dist + Distance_delta * cos(Angle + Angle_delta/2);
		    y_dist = y_dist + Distance_delta * sin(Angle + Angle_delta/2);
		    Angle = Angle + Angle_delta;
  		    t100ms_index = 7;
		    break;
		case 7: 
		    current_time = ros::Time::now();
		    //compute odom of robot 
		    odom_quat = tf::createQuaternionMsgFromYaw(Angle);
	            //Pub TF
		    odom_trans.header.stamp = current_time;
		    odom_trans.header.frame_id = "odom";
		    odom_trans.child_frame_id = "bada_chasis";
	
		    odom_trans.transform.translation.x = x_dist;
		    odom_trans.transform.translation.y = y_dist;
		    odom_trans.transform.translation.z = 0.0;
		    odom_trans.transform.rotation = odom_quat;
					
		    //Send the TF
		    odom_broadcaster.sendTransform(odom_trans);
		
		    //Pub Odometry 
		    odom.header.stamp = current_time;
		    odom.header.frame_id = "odom";
	
	            odom.pose.pose.position.x = x_dist;
		    odom.pose.pose.position.y = y_dist;
		    odom.pose.pose.position.z = 0.0;
		    odom.pose.pose.orientation = odom_quat;
	
		    //set the velocity
		    odom.child_frame_id = "bada_chasis";
		    odom.twist.twist.linear.x  = Linear_Vel;
		    odom.twist.twist.linear.y  = 0.0 ;
		    odom.twist.twist.angular.z = Angular_Vel;
	
		    bada_odom.publish(odom);
		    t100ms_index = 8;
		    break;
		case 8:
                   MotorVelControling();
                  
		   t100ms_index = 9;
		   break;
		case 9:
		    t100ms_index = 0;
		    break;
	  	defalut:
		    t100ms_index = 0;
		    break;
	    }//Switch END
	    //For Debugging
            PubMotRLSpd(&bada_MotorR, &bada_MotorL);
        }//if END
    }//while END

    R_Motor.MotorCtrl(true, 0);
    L_Motor.MotorCtrl(true, 0);
    pigpio_stop(pinum);
    return 0;
} 


void Initialize(){
    R_Motor.PIDgainSET(2.5,0.35,0.6);
    L_Motor.PIDgainSET(2.5,0.35,0.6); 

    ROS_INFO("Initialize Complete~");
    ROS_INFO("---BADA Node Start---");
}


void TwistToMotor_CB(const geometry_msgs::Twist &msg){
    double dx = 0, dy = 0, dr = 0;
    double RIGHT_V = 0, LEFT_V = 0; 

    dx = msg.linear.x;
    dy = msg.linear.y;
    dr = msg.angular.z;
   
    RIGHT_V = dx + dr * WHEELBASE / 2000;
    LEFT_V = dx - dr * WHEELBASE / 2000;
   
    R_Motor.SetVel_Target(RIGHT_V);
    L_Motor.SetVel_Target(LEFT_V);
}

void timer_CB(const ros::TimerEvent &event){
	t100ms_flag = true;
}

void MotorVelControling(){
    R_Motor.EncoderDiff();
    L_Motor.EncoderDiff();
    R_Motor.CalVel();
    L_Motor.CalVel();
    R_Motor.PIDUpdate();
    L_Motor.PIDUpdate();
}

void PubMotRLSpd(ros::Publisher *R, ros::Publisher *L ) {
    float SpdR, SpdL;
    std_msgs::Float64 SpdR_F;
    std_msgs::Float64 SpdL_F;

    SpdR = R_Motor.VELOCITY;
    SpdL = L_Motor.VELOCITY;

    SpdR_F.data = SpdR;
    SpdL_F.data = SpdL;
    R->publish(SpdR_F);
    L->publish(SpdL_F);

}
