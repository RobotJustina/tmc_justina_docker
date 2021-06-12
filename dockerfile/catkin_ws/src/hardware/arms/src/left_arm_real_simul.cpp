#include <ros/ros.h>
#include <vector>
//#include <hardware_tools/DynamixelManager.hpp>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Bool.h>
#include <sensor_msgs/JointState.h>
#include <tf/transform_broadcaster.h>

bool newGoalPose = false;
bool simul = false;

int goalPos[7] = {0, 0, 0, 0, 0, 0, 0};
int goalSpeeds[7] = {0, 0, 0, 0, 0, 0, 0};
int goalGripper[2] = {0, 0};

float goalPos_simul[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
float goalSpeeds_simul[7] = {0.1, 0.1, 0.1, 0.1, 0.1, 0.1, 0.1};
float goalGripper_simul = 0;

//int zero_arm[7] = {1543, 1694, 1742, 2100, 2048, 2066, 1050};
int zero_arm[7] = {1543, 1905, 1742, 2100, 2048, 2066, 1050};
// int zero_gripper[2] = {2440, 2680}; //To the old gripper
int zero_gripper[2] = {2187, 2680}; // To the new gripper

bool torqueGripperCCW1 = true, torqueGripperCCW2 = false, gripperTorqueActive = false, newGoalGripper = true;
float torqueGripper;
uint16_t speedGripper = 200;
uint16_t currentLoadD21, currentLoadD22;

int attempts = 0;

bool validateCMD[10] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0};

void callbackArmGoalPose(const std_msgs::Float32MultiArray::ConstPtr &msg){
    //std::cout << "left_arm_node.-> Reciving new goal left arm pose." << std::endl;
    if(!(msg->data.size() == 7 || msg->data.size() == 14))
        std::cout << "Can not process the goal poses for the left arm" << std::endl;
    else{
        if(!simul){
        }
        else{
            for(int i =0; i<7; i++){
                goalPos_simul[i] = msg->data[i];
                goalSpeeds_simul[i] = 0.1;
            }
            if(msg->data.size() == 14){
                for(int i = 0; i<7;i++)
                    goalSpeeds_simul[i] = msg->data[i+7];
            }

        }
        //newGoalPose = true;
    }
}

void callbackGripperPos(const std_msgs::Float32::ConstPtr &msg){
    if(!simul){
    }
    else
        goalGripper_simul = msg->data;
}

void callbackGripperTorque(const std_msgs::Float32::ConstPtr &msg){
    float torque = msg->data;
    if(!simul){
    }
    else
        std::cout << "Left arm torqueMode gripper" << std::endl;
}

void callback_simulated(const std_msgs::Bool::ConstPtr &msg){
    simul = msg->data;
}

int main(int argc, char ** argv){

    ros::init(argc, argv, "left_arm_node");
    ros::NodeHandle n;

    std::string port;
    int baudRate;
    bool bulkEnable = false;
    bool syncWriteEnable = false;
    bool correctParams = false;
    bool enableTorque = true;
    simul = false;

    if(ros::param::has("~port")){
        ros::param::get("~port", port);
        correctParams = true;
    }
    if(ros::param::has("~baud")){
        ros::param::get("~baud", baudRate);
        correctParams &= true;
    }
    else
        correctParams &= true;

    if(ros::param::has("~enable_torque"))
    	ros::param::get("~enable_torque", enableTorque);

    if(ros::param::has("~bulk_enable"))
        ros::param::get("~bulk_enable", bulkEnable);

    if(ros::param::has("~sync_write_enable"))
        ros::param::get("~sync_write_enable", syncWriteEnable);

    if(ros::param::has("~simul"))
        ros::param::get("~simul", simul);

    if(!correctParams){
        std::cerr << "Can not initialized the arm left node, please put correct params to this node, for example." << std::endl;
        std::cerr << "port : tty01" << std::endl;
        std::cerr << "baud : 1000000" << std::endl;
        return -1;
    }

    tf::TransformBroadcaster broadCaster;

    ros::Subscriber subGoalPos = n.subscribe("/hardware/left_arm/goal_pose", 1, callbackArmGoalPose);
    ros::Subscriber subGripperPos = n.subscribe("/hardware/left_arm/goal_gripper", 1, callbackGripperPos);
    ros::Subscriber subGripperTroque = n.subscribe("/hardware/left_arm/torque_gripper", 1, callbackGripperTorque);
    ros::Subscriber subSimul = n.subscribe("/simulated", 1, callback_simulated); 

    ros::Publisher joint_pub = n.advertise<sensor_msgs::JointState>("/joint_states", 1);
    ros::Publisher pubArmPose = n.advertise<std_msgs::Float32MultiArray>("left_arm/current_pose", 1);
    ros::Publisher pubGripper = n.advertise<std_msgs::Float32>("left_arm/current_gripper", 1);
    ros::Publisher pubObjOnHand = n.advertise<std_msgs::Bool>("left_arm/object_on_hand", 1);
    ros::Publisher pubBattery = n.advertise<std_msgs::Float32>("/hardware/robot_state/left_arm_battery", 1);

    ros::Rate rate(50);

    std::vector<int> ids;
    for(int i = 0; i < 7; i++)
        ids.push_back(i);
    if(!simul){
    }

    //uint16_t curr_position[9] = {1543, 1600, 1800, 2100, 2048, 1800, 1050, 2440, 2680};
    //uint16_t curr_position[9] = {1543, 1694, 1742, 2100, 2048, 2066, 1050, 2440, 2680};
    uint16_t curr_position[9] = {1543, 1777, 1742, 2100, 2048, 2066, 1050, 2440, 2680};

    //float bitsPerRadian = (4095.0)/((360.0)*(3.141592/180.0));
    float bitsPerRadian = 4095.0/360.0*180.0/M_PI;

    std::string names[9] = {"la_1_joint", "la_2_joint", "la_3_joint", "la_4_joint", "la_5_joint", "la_6_joint", "la_7_joint", "la_grip_left", "la_grip_right"};
    float positions[9] = {0, 0, 0, 0, 0, 0, 0, 0, 0};

    sensor_msgs::JointState jointStates;
    jointStates.name.insert(jointStates.name.begin(), names, names + 9);
    jointStates.position.insert(jointStates.position.begin(), positions, positions + 9);

    // Setup features for init the servos of left arm
    if(!simul && enableTorque){
    }

    goalGripper[0] = zero_gripper[0];
    goalGripper[1] = zero_gripper[1];

    std_msgs::Float32MultiArray msgCurrPose;
    msgCurrPose.data.resize(7);
    std_msgs::Float32 msgCurrGripper;
    std_msgs::Bool msgObjOnHand;
    std_msgs::Float32 msgBattery;


    //initialize simulation variables
    float Pos[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float deltaPos[7] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
    float deltaGripper = 0.0;
    float gripper = 0.0;
    float gripperSpeed = 0.1;
    for(int i = 0; i < 7; i++ ){
        goalPos_simul[i] = 0.0;
        goalSpeeds_simul[i] = 0.1;
    }
    goalGripper_simul = 0.0;
    bool flagOnce = true;

    while(ros::ok()){
        if(!simul){
        }
        else{
            for(int i = 0; i < 7; i++){
                deltaPos[i] = goalPos_simul[i] - Pos[i];
                if(deltaPos[i] > goalSpeeds_simul[i])
                    deltaPos[i] = goalSpeeds_simul[i];
                if(deltaPos[i] < -goalSpeeds_simul[i])
                    deltaPos[i] = -goalSpeeds_simul[i];
                Pos[i] += deltaPos[i];
                jointStates.position[i] = Pos[i];
                msgCurrPose.data[i] = Pos[i]; 
            }
            deltaGripper = goalGripper_simul -gripper;
            if(deltaGripper > gripperSpeed)
                deltaGripper = gripperSpeed;
            if(deltaGripper < -gripperSpeed)
                deltaGripper = -gripperSpeed;
            gripper += deltaGripper;
            jointStates.position[7] = gripper;
            jointStates.position[8] = gripper;
            msgCurrGripper.data = gripper; 

            jointStates.header.stamp = ros::Time::now();
            joint_pub.publish(jointStates);
            pubArmPose.publish(msgCurrPose);
            pubGripper.publish(msgCurrGripper);
            //pubObjOnHand.publish(msgObjOnHand);
            msgBattery.data =  11.6;
            pubBattery.publish(msgBattery);

        }

        if(ros::param::has("~enable_torque"))
            ros::param::get("~enable_torque", enableTorque);
        
        rate.sleep();
        ros::spinOnce();
    }

    std::cout << "left_arm_pose.->Shutdowning the left arm node" << std::endl;
    std::cout << "left_arm_pose.->Writing the zero_arm init pose" << std::endl;

    return 1;
}

