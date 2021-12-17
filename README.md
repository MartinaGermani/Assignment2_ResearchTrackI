# Assignment2 - ROS C++


## How to run:



## Explanation of the code structure:
The aim of this second assignment is to drive the robot around the circuit, avoiding that it encounters the obstacles, represented by the edges of the circuit. 
To do that I have used ROS, and the code is written in C++.

The code is composed by two nodes, which are as follows:
```c++
ros::init(argc, argv, "robot_control");  
````

```c++
ros::init(argc, argv, "UI_node"); 
````
where the first one controls the robot and also increments or decrements the velocity of the robot, as a service; whereas the second one constantly waits for an input for the user,as a client, that may ask to increment or decrement the robot's velocity or to put the robot in the initial position.

So, I have created two script .cpp, one for each node, and moreover I have created a Custom Service .svr, that allows the communication between server and client.

For controlling the robot, I have worked considering the distance between the robot and the obstacles around and in front of it: informations that are provided by /base_scan topic, in particular in "ranges" vector, that has 720 elements. Wherease, to modify the robot's velocity I have used /cmd_vel topic, that contains information about the linear and the angular velocity of the robot. 
So, I have defined two subscribers(one for the distance and one for the velocity), and also a publisher in order to publish the velocity on the /cmd_vel topic:
```c++
  ros::Subscriber sub = nh.subscribe("/cmd_vel", 10, robotCallback);  
	
  ros::Subscriber scan_sub = nh.subscribe("/base_scan",10, distCallback);
  
  ros::Publisher pub =  nh.advertise<geometry_msgs::Twist> ("/cmd_vel", 10);
````
## Pseudo-Code
Once the control node is executed, the robot starts moving in the circuit according to the following logic:

![alt text](https://github.com/MartinaGermani/Assignment2_ResearchTrackI/blob/main/flowchart_control.jpg?raw=true
