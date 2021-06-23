//Bibliotecas por default

#include <iostream>
#include "ros/ros.h"
#include "justina_tools/JustinaHardware.h"
#include "justina_tools/JustinaManip.h"
#include "justina_tools/JustinaNavigation.h"
#include "justina_tools/JustinaTools.h"
#include "justina_tools/JustinaVision.h"
#include "justina_tools/JustinaTasks.h"
//Estados para la máquina

enum SMState {
    SM_INIT,
	SM_INIT_TABLE,
	SM_WAIT_FOR_COMMAND,
	SM_REPEAT_COMMAND,
	SM_PARSE_SPOKEN_COMMAND,
    SM_WAIT_COMMAND,
    SM_RESET_OBJ,
    SM_GET_ORDER,
    SM_WHERE_IS,
    SM_SELECT_OBJECT_GRASP,
	SM_NAVIGATE_TO_INSPECTION,
    SM_NAVIGATE_LOCATION,
	SM_ALIGN_TABLE,
	SM_DETECT_OBJECT,
    SM_NO_DETECT_OBJECT,
	SM_HANDLER,
	SM_GRASP_OBJECT,
	SM_DELIVER_OBJECT,
	SM_REPEAT_TASK,
	SM_FINAL_STATE
};

//delivery locations for each category
enum objType {None,Food, Kitchen, Tools, Shape};

std::string get_location(objType type) {
   switch(type) {
      case Food:
         return "long_table_a_2";
      case Kitchen:
         return "Container_A";
      case Tools:
         return "Drawer_top";
      case Shape:
         return "Drawer_left";
      default:
         return "Invalid type";
   }
}
//-------arms
//right
int right_obj_index=-1;
objType right_obj_type=None;
//left
int left_obj_index=-1;
objType left_obj_type=None;
//-------SM
SMState state = SM_INIT;
bool table_aligned = false;
//********************
int main(int argc, char** argv){
	std::cout << "INITIALIZING ACT_PLN BY MARCOSOFT..." << std::endl; //cout
    ros::init(argc, argv, "act_pln");
    ros::NodeHandle n;
    //Setting nodeHandle
    JustinaHardware::setNodeHandle(&n);
    JustinaManip::setNodeHandle(&n);
    JustinaNavigation::setNodeHandle(&n);
    JustinaTasks::setNodeHandle(&n);
    JustinaTools::setNodeHandle(&n);
    JustinaVision::setNodeHandle(&n);
    ros::Rate loop(10);

    int nextState = 0;
    bool fail = false;
    bool success = false;
    std::vector<vision_msgs::VisionObject> recoObj;
    sensor_msgs::Image image;
    int index;
    bool la = false;
    bool ra = false;
    bool drop = false;

    while(ros::ok() && !fail && !success){
    
        switch(state){
        	//initial state
            case SM_INIT:
        		std::cout << "State machine: SM_INIT" << std::endl;	
        		state = SM_NAVIGATE_TO_INSPECTION;
            break;
            //move to the objects initial position
        	case SM_NAVIGATE_TO_INSPECTION:
        		//Go to location
        		std::cout << "State machine: SM_NAVIGATE_TO_INSPECTION" << std::endl;
			    //go to tall_table because is the easiest path
                if(!JustinaNavigation::getClose("tall_table", 120000)){
                    std::cout << "Cannot move to tall_table" << std::endl;
                }
                
                state = SM_ALIGN_TABLE;
			    
        	    break;	
            //Align and get close to the table
    		case SM_ALIGN_TABLE:
    			std::cout << "State machine: SM_ALIGN_TABLE" << std::endl;
                std::cout << "I aling with the tall table" << std::endl;
    			JustinaManip::torsoGoTo(0.0, 0.0, 0.0, 6000);
        		table_aligned = JustinaTasks::alignWithTable(0.2);
                JustinaNavigation::moveDist(0.4, 4000);
        		
        		state = SM_DETECT_OBJECT;
    			break;
                
			//search for an object for each arm
            case SM_DETECT_OBJECT:
    			std::cout << "State machine: SM_DETECT_OBJECT" << std::endl;
                //try to find an object     
                if(JustinaVision::detectAllObjectsVot(recoObj, image, 5)){
                    for(int j = 0; j < recoObj.size() && !table_aligned; j++){
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
	        	state = SM_GRASP_OBJECT;
    		    break;
            //if and object has been detected try to grasp it
            case SM_GRASP_OBJECT:
			    std::cout << "State machine: SM_GRASP_OBJECT" << std::endl;

                //try to grasp objects with selected arm
                
                if(left_obj_index>=0){
                    JustinaTasks::graspObject(recoObj[left_obj_index].pose.position.x, recoObj[left_obj_index].pose.position.y, recoObj[left_obj_index].pose.position.z, true, "", true);
                    drop = false;                		
                }
                if(right_obj_index>=0){
                    JustinaTasks::graspObject(recoObj[right_obj_index].pose.position.x+0.2, recoObj[right_obj_index].pose.position.y, recoObj[right_obj_index].pose.position.z, false, "", true);
                    drop = true;
                }
                
				state = SM_NAVIGATE_LOCATION;		        
			    break;
            //go to deliver zone
            case SM_NAVIGATE_LOCATION:
        		//Go to location
        		std::cout << "State machine: SM_NAVIGATE_LOCATION" << std::endl;
                //std::string target = get_location(right_obj_type);
                std::cout << "Moving to " << get_location(right_obj_type)<< std::endl;
                if(!JustinaNavigation::getClose(get_location(right_obj_type), 120000)){
                    std::cout << "Cannot move to "<<get_location(right_obj_type) << std::endl;
                }
                JustinaNavigation::moveDist(0.4, 4000);
                state = SM_DELIVER_OBJECT;
			    
        	    break;

            case SM_DELIVER_OBJECT:
				std::cout << "State machine: SM_DELIVER_OBJECT" << std::endl;
				
                if(right_obj_index>=0){
                	JustinaManip::raGoTo("take", 3000);
                	JustinaTasks::dropObject("", false, 10000);
            	}
            	if(left_obj_index>=0){
            		JustinaManip::laGoTo("take", 3000);
            		JustinaTasks::dropObject("", true, 10000);
            	}
     
                JustinaManip::hdGoTo(0.0, 0.0, 6000);
                //contdrink++;
                state = SM_FINAL_STATE;
		        break;
                
            case SM_FINAL_STATE:
        		//Final state
        		std::cout << "State machine: SM_FINAL_STATE" << std::endl;	

                JustinaNavigation::getClose("initial_point", 120000);
                //JustinaHRI::waitAfterSay("I have finished test",4000);
			    std::cout << "I have finished test" << std::endl;	
        		success = true;
        		fail = true;
                break;
        }
        ros::spinOnce();
        loop.sleep();
    }
    return 0;
}