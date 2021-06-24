#include "ros/ros.h"
#include "justina_tools/JustinaHRI.h"
#include "justina_tools/JustinaHardware.h"
#include "justina_tools/JustinaManip.h"
#include "justina_tools/JustinaNavigation.h"
#include "justina_tools/JustinaTools.h"
#include "justina_tools/JustinaVision.h"
#include "justina_tools/JustinaTasks.h"

#define SM_WAIT_FOR_COMMAND 0
#define SM_INIT             10
#define SM_GO_TO_SHELF      20
#define SM_ALIGN_TO_SHELF   30
#define SM_DETECT_OBJECT    40
#define SM_GRASP_OBJECT     50
#define SM_FINISH_TEST      100


std::string stateMachine = "go_and_get.->";
enum objType {None,Food, Kitchen, Tools, Shape};


//-------arms
//right
int right_obj_index=-1;
objType right_obj_type=None;
//left
int left_obj_index=-1;
objType left_obj_type=None;

int main(int argc, char** argv) {

    std::cout << "Starting ACT_PLN GO AND GET IT by Luis Nava" << std::endl;

    ros::init(argc, argv, "go_and_get");
    ros::NodeHandle nh;
    JustinaHardware::setNodeHandle(&nh);
    JustinaManip::setNodeHandle(&nh);
    JustinaNavigation::setNodeHandle(&nh);
    JustinaTasks::setNodeHandle(&nh);
    JustinaTools::setNodeHandle(&nh);
    JustinaVision::setNodeHandle(&nh);
    ros::Rate rate(10);

    bool fail = false;
    bool success = false;

    int nextState = 0;

    bool shelf_aligned = false;
    
    sensor_msgs::Image image;
    std::vector<vision_msgs::VisionObject> recoObj;
    
    int index = 0;

    while(ros::ok() && !fail && !success) {
        
        switch(nextState) {
            case SM_WAIT_FOR_COMMAND:
            {
                std::cout << stateMachine << "\033[1;33mSM_WAIT_FOR_COMMAND\033[0m\n" << std::endl;
                
                JustinaManip::startHdGoTo(0.0, 0.0);
                JustinaHRI::say("I'm ready for go and get it test");
                std::cout << "\033[1;34mI am waiting for the door to be open\033[0m\n" << std::endl;
                nextState = SM_INIT;
            }
            break;
            
            case SM_INIT:
            {
                std::cout << stateMachine << "\033[1;33mSM_INIT\033[0m\n" << std::endl;
                nextState = SM_GO_TO_SHELF;
            }
            break;

            case SM_GO_TO_SHELF:
            {
                
                std::cout << stateMachine << "\033[1;33mSM_GO_TO_SHELF\033[0m\n" << std::endl;
                if(!JustinaNavigation::getClose("shelf", 120000)){
        			std::cout << "Cannot move to shelf" << std::endl;
        		}
                nextState = SM_ALIGN_TO_SHELF;
            }
            break;

    		case SM_ALIGN_TO_SHELF:
            {
                std::cout << stateMachine << "\033[1;33mSM_ALIGN_TO_SHELF\033[0m\n" << std::endl;
                std::cout << "I aling with the tall shelf" << std::endl;
    			JustinaManip::torsoGoTo(0.0, 0.0, 0.0, 6000);
        		shelf_aligned = JustinaTasks::alignWithTable(0.2);
                JustinaNavigation::moveDist(0.4, 4000);
        		
        		nextState = SM_DETECT_OBJECT;
    			break;
            }

            case SM_DETECT_OBJECT:
            {
                std::cout << stateMachine << "\033[1;33mSM_DETECT_OBJECT\033[0m\n" << std::endl;

                //try to find an object     
                if(JustinaVision::detectAllObjectsVot(recoObj, image, 5)){
                    for(int j = 0; j < recoObj.size() && !shelf_aligned; j++){
                        // id.rfind("",0) search by prefix
                        //Food category
                        if (recoObj[j].id.rfind("Food", 0) == 0){
                            //select arm
                            //left
                            if(recoObj[index].pose.position.y > 0 && left_obj_index<0){
                                left_obj_type=Food;
                                left_obj_index=j;
                                std::cout<< "Selecting "<<recoObj[j].id<<" with left arm"<<std::endl;
                            }
                            //right
                            else if(right_obj_index<0){
                                right_obj_type=Food;
                                right_obj_index=j;
                                std::cout<< "Selecting "<<recoObj[j].id<<" with right arm"<<std::endl;
                            }
                        }
                    }
                }
                //nextState = SM_FINISH_TEST;
            }
            if(right_obj_index<0 && left_obj_index<0){
                //small move to try again...
                nextState = SM_ALIGN_TO_SHELF;
            }else{
	            nextState = SM_GRASP_OBJECT;
            }
            break;

            //if and object has been detected try to grasp it
            case SM_GRASP_OBJECT:
            {
                std::cout << "State machine: SM_GRASP_OBJECT" << std::endl;

                //try to grasp objects with selected arm
                JustinaManip::torsoGoTo(0,0,0,5000);
                if(left_obj_index>=0){
                    JustinaTasks::graspObject(recoObj[left_obj_index].pose.position.x, recoObj[left_obj_index].pose.position.y, recoObj[left_obj_index].pose.position.z, true, "", false);
                    nextState = SM_FINISH_TEST;               		
                }
                if(right_obj_index>=0){
                    JustinaTasks::graspObject(recoObj[right_obj_index].pose.position.x, recoObj[right_obj_index].pose.position.y, recoObj[right_obj_index].pose.position.z, false, "", true);
                    if (nextState != 10){
                        nextState = SM_FINISH_TEST;  
                    }
                }
            }
		    break;

            case SM_FINISH_TEST:
            {
                std::cout << stateMachine << "\033[1;33mSM_FINISH_TEST.-> I have finish the test\033[0m\n" << std::endl;
                //JustinaHRI::say("I have finished the test");
                fail = false;
                success = true;
            }
            break;


            default:{
                    fail = true;
                    success = true;
            }
                break;
        }


        ros::spinOnce();
        rate.sleep();
    }

    return 0;
}