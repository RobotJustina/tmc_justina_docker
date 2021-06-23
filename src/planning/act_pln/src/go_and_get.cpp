#include "ros/ros.h"
#include "justina_tools/JustinaHRI.h"
#include "justina_tools/JustinaHardware.h"
#include "justina_tools/JustinaManip.h"
#include "justina_tools/JustinaNavigation.h"
#include "justina_tools/JustinaTools.h"
#include "justina_tools/JustinaVision.h"
#include "justina_tools/JustinaTasks.h"

#define SM_WAIT_FOR_COMMAND 0
#define SM_INIT 10
#define SM_NAVIGATE_TO_INSPECTION 20
#define SM_FINISH_TEST 100


std::string stateMachine = "go_and_get.->";



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
                nextState = SM_NAVIGATE_TO_INSPECTION;
            }
            break;

            case SM_NAVIGATE_TO_INSPECTION:
            {
                std::cout << stateMachine << "\033[1;33mSM_NAVIGATE_TO_INSPECTION\033[0m\n" << std::endl;
                if(!JustinaNavigation::getClose("tall_table", 120000)){
        			std::cout << "Cannot move to tall_table" << std::endl;
        		}
                nextState = SM_FINISH_TEST;
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