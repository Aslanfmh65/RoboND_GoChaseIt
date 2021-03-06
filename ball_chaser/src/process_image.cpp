#include "ros/ros.h"
#include "ball_chaser/DriveToTarget.h"
#include <sensor_msgs/Image.h>

// Define a global client that can request services
ros::ServiceClient client;

// This function calls the command_robot service to drive the robot in the specified direction
void drive_robot(float lin_x, float ang_z)
{
    ball_chaser::DriveToTarget srv;

    srv.request.linear_x = lin_x;
    srv.request.angular_z = ang_z;

    if ( !client.call(srv) )
    {
        ROS_ERROR( "Failed to call service ball_chaser" );
    }
}

// This callback function continuously executes and reads the image data
void process_image_callback(const sensor_msgs::Image img)
{

    int white_pixel = 255;
    int left = img.step / 3;
    int right = left * 2;
    bool ball_found = false;
    float forward_speed = 2;
    float turning_speed = 0.5;
    float stop = 0.0;
    float turn_left = 4.0;
    float turn_right = -4.0;

    for (int i = 0; i < img.height * img.step; ++i)
    {
       if ( white_pixel == img.data[i] ) 
       {
            //ROS_INFO("Found ball row: %d, column: %d", row/img.width, i%img.step);
            ball_found = true;

            if ( i%img.step < left )
            {
                ROS_INFO("Turning Left");
                drive_robot(turning_speed, turn_left);
            }
            else if ( i%img.step > right )
            {
                ROS_INFO("Turning Right");
                drive_robot(turning_speed, turn_right);
            }
            else
            {
                ROS_INFO("Driving Straight");
                drive_robot( forward_speed, stop);
            }
            break;
       }
    }

    if ( ball_found == false )
    {
        drive_robot(stop, stop);
    }
}

int main(int argc, char** argv)
{
    // Initialize the process_image node and create a handle to it
    ros::init(argc, argv, "process_image");
    ros::NodeHandle n;

    // Define a client service capable of requesting services from command_robot
    client = n.serviceClient<ball_chaser::DriveToTarget>("/ball_chaser/command_robot");

    // Subscribe to /camera/rgb/image_raw topic to read the image data inside the process_image_callback function
    ros::Subscriber sub1 = n.subscribe("/camera/rgb/image_raw", 10, process_image_callback);

    // Handle ROS communication events
    ros::spin();

    return 0;
}
