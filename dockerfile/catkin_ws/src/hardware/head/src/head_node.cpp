#include <ros/ros.h>
#include <vector>
#include <std_msgs/Float32MultiArray.h>
#include <std_msgs/Float32.h>
#include <std_msgs/Bool.h>
#include <sensor_msgs/JointState.h>
#include <tf/transform_broadcaster.h>

bool newGoalPose = true;
bool readSimul = false;
bool simul = false;

int goalPos[2] = {0, 0};
int goalSpeeds[2] = {90, 75};
int PID[2][3] = {{24, 0, 128}, {64, 0, 32}};
int minLimits[2] = {1023, 0};
int maxLimits[2] = {3069, 4095};

float goalPos_simul[2] = {0.0, 0.0};
float goalSpeeds_simul[2] = {0.3, 0.3};

int zero_head[2] = {2040, 2520};
//float offset = -0.07671; // This for help me carry
//float offset = -0.10671; // This for help me carry
float offset = 0.0;
//float offset = -0.04; // This is for p and g 
float offsetReadSimul = -0.04;

void callbackHeadGoalPose(const std_msgs::Float32MultiArray::ConstPtr &msg){
    // std::cout << "head_node.-> Reciving new goal head pose." << std::endl;
    if(!(msg->data.size() == 2))
        std::cout << "Can not process the goal poses for the head" << std::endl;
    else{
        if(!simul ||(!simul && readSimul)){
        }
        else{
            goalPos_simul[0] = msg->data[0];
            goalSpeeds_simul[0] = 0.1;
            goalPos_simul[1] = msg->data[1];
            goalSpeeds_simul[1] = 0.1;
        }
    }
}

void callback_simulated(const std_msgs::Bool::ConstPtr &msg){
    simul = msg->data;
}

int main(int argc, char ** argv){

    ros::init(argc, argv, "head_node");
    ros::NodeHandle n;
    
    std::string port;
    int baudRate;
    bool bulkEnable = false;
    bool syncWriteEnable = false;
    bool correctParams = false;
    offset = 0.0;
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

    if(ros::param::has("~head_offset"))
    	ros::param::get("~head_offset", offset);

    if(ros::param::has("~bulk_enable"))
        ros::param::get("~bulk_enable", bulkEnable);
    
    if(ros::param::has("~sync_write_enable"))
        ros::param::get("~sync_write_enable", syncWriteEnable);

    if(ros::param::has("~simul"))
        ros::param::get("~simul", simul);
    
    if(ros::param::has("~read_simul"))
        ros::param::get("~read_simul", readSimul);

    if(!correctParams){
        std::cerr << "Can not initialized the head node, please put correct params to this node, for example." << std::endl;
        std::cerr << "port : tty01" << std::endl;
        std::cerr << "baud : 1000000" << std::endl;
        return -1;
    }

    ros::Subscriber subGoalPos = n.subscribe("/hardware/head/goal_pose", 1, callbackHeadGoalPose);
    ros::Subscriber subSimul = n.subscribe("/simulated", 1, callback_simulated); 
    ros::Publisher joint_pub = n.advertise<sensor_msgs::JointState>("/joint_states", 1);
    ros::Publisher pubHeadPose = n.advertise<std_msgs::Float32MultiArray>("head/current_pose", 1);
    ros::Publisher pubBattery = n.advertise<std_msgs::Float32>("/hardware/robot_state/head_battery", 1);

    ros::Rate rate(30);

    std::vector<int> ids;
    ids.push_back(0);
    ids.push_back(1);
    if(!simul){
    }

    uint16_t curr_position[2];
    for(int i = 0; i < 2; i++){
        curr_position[i] = zero_head[i];
        goalPos[i] = zero_head[i];
    }

    float bitsPerRadian = 4095.0/360.0*180.0/M_PI;

    std::string names[2] = {"pan_connect", "tilt_connect"};
    float positions[2] = {0, 0};
    
    sensor_msgs::JointState jointStates;
    jointStates.name.insert(jointStates.name.begin(), names, names + 2);
    jointStates.position.insert(jointStates.position.begin(), positions, positions + 2);

    // Setup features for init the servos of left arm
    if(!simul){
    }

    std_msgs::Float32MultiArray msgCurrPose;
    msgCurrPose.data.resize(2);
    std_msgs::Float32 msgBattery;
    

    //initialize simulation variables
    float Pos[2] = {0.0, 0.0};
    float deltaPos[2] = {0.0, 0.0};
    for(int i = 0; i < 2; i++){
        goalPos_simul[i] = 0.0;
        goalSpeeds_simul[i] = 0.1;
    }

    while(ros::ok()){
        if(!simul){
        }
        if(!simul && !readSimul){
        }
        else{
            for(int i = 0; i < 2; i++){
                deltaPos[i] = goalPos_simul[i] - Pos[i];
                if(deltaPos[i] > goalSpeeds_simul[i])
                    deltaPos[i] = goalSpeeds_simul[i];
                if(deltaPos[i] < -goalSpeeds_simul[i])
                    deltaPos[i] = -goalSpeeds_simul[i];
                Pos[i] += deltaPos[i];
                if(i == 0)
                    jointStates.position[i] = Pos[i];
                else{ 
                    if(readSimul)
                        jointStates.position[i] = -Pos[i] + offsetReadSimul;
                    else
                        jointStates.position[i] = -Pos[i];
                }
                msgCurrPose.data[i] = Pos[i]; 
            }
                
            jointStates.header.stamp = ros::Time::now();
            joint_pub.publish(jointStates);
            pubHeadPose.publish(msgCurrPose);
            msgBattery.data =  11.6;
            pubBattery.publish(msgBattery);

        }

        rate.sleep();
        ros::spinOnce();
    }

    std::cout << "head_node.->Shutdowning the head node" << std::endl;
    std::cout << "head_node.->Writing the shutdowning pose" << std::endl;

    return 1;
}

