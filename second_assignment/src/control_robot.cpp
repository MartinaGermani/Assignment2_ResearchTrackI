#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"
#include "std_srvs/Empty.h"
#include "my_svr/Vel.h"

// Initialization as global variables:

char a;

geometry_msgs::Twist msg;
geometry_msgs::Twist msg2;

float acc=0;
float dec=0;
float reset_command=1;

my_svr::Vel::Request req;
my_svr::Vel::Response res;

std_srvs::Empty reset;


ros::Publisher pub;


ros::ServiceServer server;
ros::ServiceClient client_reset;



bool robotservice(my_svr::Vel::Request &req, my_svr::Vel::Response &res){
	acc=req.x*req.a;
	dec=req.x*req.d;
	reset_command=req.x*req.r;
	res.a=acc;
	res.d=dec;
	//res.r=reset_command;
	return true;
}

//The following callback controls the distance between the robot and the obstacles using the /base_scan topic, that returns the distance, computed by the laser, between the robot and the obstacols in front and around it.
//In particular, for controlling the robot, the information about the distances is in "ranges" field.
//Moreover, in this function, the speed of the robot is also accelerated or decreased, if requested by the user, thanks to the comunication between client and server.

void distCallback(const sensor_msgs::LaserScan::ConstPtr& scan)
{
	float value_360 = scan->ranges[360];
	float value_180 = scan->ranges[180];
	float value_720 = scan->ranges[720];
	float value_540 = scan->ranges[540];
	float value_0 = scan->ranges[0];
	
	
	ROS_INFO("at 0 degrees: %f", value_0);
	ROS_INFO("at 45 degrees: %f", value_180);
	ROS_INFO("at 90 degrees: %f", value_360);
	ROS_INFO("at 135 degrees: %f", value_540);
	ROS_INFO("at 180 degrees: %f", value_720);
	
	
	msg.linear.x=1.0;
	
//when the user wants to increment the velocity of the robot, the service is going to increment it, and so acc will becom different from zero, and so in this condition the current velocity of the robot is modified, and then acc is setted to 0 again.

	if(acc!=0)
	{
		msg.linear.x=acc; //increase the speed
		ROS_INFO("x: %.2f, y: %.2f, z: %.2f",  
	msg.linear.x, msg.linear.y, msg.linear.z);
		acc=0;
	}
	
//when the user wants to decrement the velocity of the robot, the service is going to decrement it, and so dec will becom different from zero, and so in this condition the current velocity of the robot is modified, and then dec is setted to 0 again.
	else if(dec!=0)
	{
		msg.linear.x=dec; //decrease the speed
		ROS_INFO("x: %.2f, y: %.2f, z: %.2f",  
	msg.linear.x, msg.linear.y, msg.linear.z);
		dec=0;
	}

//if the obstacle in front of te robot is close (frontal distance less than 2.6) and the obtacle on the left is close, than the robot decreases its velocity and turns right 45 degrees.

	else if(scan->ranges[360]<2.6 && scan->ranges[540]<1 && scan->ranges[180]>1)
		{
			ROS_INFO("attenzione gira a destraa");
			msg.linear.x=0.3;
			msg.linear.y=0.0;
			msg.angular.z=-0.45;
			ROS_INFO("x: %.2f, y: %.2f, z: %.2f",  
	msg.linear.x, msg.linear.y, msg.linear.z);
		}
	
//if the obstacle in front of te robot is close (frontal distance less than 2.6) and the obtacle on the right is close, than the robot decreases its velocity and turns left 70 degrees.
	
	else if(scan->ranges[360]<2.6 && scan->ranges[180]<1 && scan->ranges[540]>1)
		{
			ROS_INFO("attenzione gira a sinistraa");
			msg.linear.x=0.3;
			msg.linear.y=0;
			msg.angular.z=0.70;
			ROS_INFO("x: %.2f, y: %.2f, z: %.2f",  
	msg.linear.x, msg.linear.y, msg.linear.z);
		}
		
//if the obstacle in front of te robot is far (frontal distance bigger than 5 and less than 8) and the obtacle on the right is close, than the robot decreases its velocity and turns left 40 degrees.
	
	else if(scan->ranges[0]<1 && scan->ranges[360]>5.0 && scan->ranges[360]<8)
		{
			ROS_INFO("attenzione gira left");
			msg.linear.x=0.5;
			msg.linear.y=0;
			msg.angular.z=0.4;
			ROS_INFO("x: %.2f, y: %.2f, z: %.2f",  
	msg.linear.x, msg.linear.y, msg.linear.z);
		}
	
//if the obstacle in front of te robot is far (frontal distance bigger than 5) and the obtacle on the left is close, than the robot decreases its velocity and turns right 40 degrees.
		
	else if(scan->ranges[180]<1 && scan->ranges[360]>5.0 && scan->ranges[360]<8)
		{
			ROS_INFO("attenzione gira right");
			msg.linear.x=0.5;
			msg.linear.y=0;
			msg.angular.z=-0.4;
			ROS_INFO("x: %.2f, y: %.2f, z: %.2f",  
	msg.linear.x, msg.linear.y, msg.linear.z);
		}		
}

//The following callback controls the velocity of the robot

void robotCallback(const geometry_msgs::Twist::ConstPtr& msg1)
	{
	
	ROS_INFO("x: %f, y: %f, z: %f",  
	msg1->linear.x, msg1->linear.y, msg1->linear.z);
	
	}

//In the main function, there are the definitions of the node, of the publishers and subscribers and of the service.

int main (int argc, char **argv)
{
// Initialization of the control node, and setup  of the NodeHandle for handling the communication with the ROS //system  
	
	
	ros::init(argc, argv, "robot_control");  
	ros::NodeHandle nh;
	
	// Define the publishers and the subscribers to robot's velocity and robot's distance
	

	pub = nh.advertise<geometry_msgs::Twist> ("/cmd_vel", 10); 
	 ros::Publisher scan_pub = nh.advertise<sensor_msgs::LaserScan>("/base_scan",1);
	
	ros::Subscriber sub = nh.subscribe("/cmd_vel", 10, robotCallback);  
	
	ros::Subscriber scan_sub = nh.subscribe("/base_scan",10, distCallback);
	
	
	// Define the server that interacts with the client to increment o decrement the velocity of the robot
	
	server=nh.advertiseService("/Vel", robotservice);	
	

	msg.linear.x=1.0;
	
	sensor_msgs::LaserScan scan; //definition of the scan object
	
	scan.ranges.resize(720);
	
	ros::Rate rate(10);
	ROS_INFO("Starting to move forward");	
		
	
	while(nh.ok()) {
		
	pub.publish(msg);
	
	ros::spinOnce();
	rate.sleep();
	}
	
	
	
	return 0;
}
