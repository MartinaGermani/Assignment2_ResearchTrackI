#include "ros/ros.h"
#include "geometry_msgs/Twist.h"
#include "sensor_msgs/LaserScan.h"
#include "std_srvs/Empty.h"
#include "my_svr/Vel.h"

using namespace std;

//Initialization of global variables:

ros::ServiceClient client;
ros::ServiceClient client_reset;

my_svr::Vel vel;

char a;

geometry_msgs::Twist msg;
geometry_msgs::Twist msg1;

std_srvs::Empty reset;


int main (int argc, char **argv)
{

//Definition of the UI node:

	ros::init(argc, argv, "UI_node");  
	ros::NodeHandle nh;
	
//Definition of the 2 client: the first one communicates with the server built by me to modify the robot's velocity, whereas the second one communicates with a server already available to reset the initial position of the robot: 

	client = nh.serviceClient<my_svr::Vel>("/Vel");
	client_reset = nh.serviceClient<std_srvs::Empty>("/reset_positions");
	
//Initialization of Custom Service's Request and Response, and their definition:

	my_svr::Vel::Request req;
	my_svr::Vel::Response resp;
	req.x=1.0;
	req.a=2.0;
	req.d=0.5;
	req.r=0.0;
	
//Wait for the service, that modifies the velocity, to be advertised and available;
	
	ros::service::waitForService("Vel",ros::Duration(2));
	
//The user must decide if to increase or decrease the velocity of the robot, or to reset the robot to the initial position:

	cout<<"put in -a to increase the velocity, -d to decrease the velocity, -r to reset the initial position of the robot"<<endl;
	cin>>a;
	
	if(a=='a')
	{
		client.call(req,resp); //call the service
		ROS_INFO("new x: %.2f", resp.a);
	}
	
	else if(a=='d')
	{
		client.call(req,resp); //call the service
		ROS_INFO("new x: %.2f", resp.d);
	}
	
	else if(a=='r')
	{
		//wait the service that reset the initial position:
		client_reset.waitForExistence(); 
		client_reset.call(reset); //call the service
		ROS_INFO("reset initial position", resp.r);
	}

	else
	{
		ROS_INFO("NO INPUT");
	}	

	return 0;	 
}
