//Bibliotecas por default

#include <iostream>
#include "ros/ros.h"
#include "justina_tools/JustinaHardware.h"
#include "justina_tools/JustinaManip.h"
#include "justina_tools/JustinaNavigation.h"
#include "justina_tools/JustinaTools.h"
#include "justina_tools/JustinaVision.h"
#include "justina_tools/JustinaTasks.h"
#include "rosgraph_msgs/Clock.h"
//Estados para la máquina

enum SMState {
    SM_INIT,
	SM_NAVIGATE_TO_TALL_TABLE,
    SM_NAVIGATE_TO_LONG_TABLE,
	SM_ALIGN_TABLE,
	SM_DETECT_OBJECT,
	SM_GRASP_OBJECT,
    SM_LEFT_NAVIGATE_LOCATION,
    SM_RIGHT_NAVIGATE_LOCATION,
	SM_RIGHT_DELIVER_OBJECT,
    SM_LEFT_DELIVER_OBJECT,
	SM_FINAL_STATE
};

//delivery locations for each category
enum objType {None,Food, Kitchen, Tool, Shape, Task};

std::string get_location(objType type) {
   switch(type) {
      case Food:
         return "long_table_a_2";
      case Kitchen:
         return "long_table_a_1";
      case Tool:
      case Task:
      case Shape:
         return "bin_a";
      case None:
      default:
         return "bin_b";
   }
}
//-------arms
objType obj_type=None;
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

    std::vector<float> world_right_pose(6);
    std::vector<float> relative_right_pose(6);

    int tall_table_attempts=0;
    bool obj_allowed=true;
    std::vector<std::string> obj_blacklist{
        "Food_cracker_box",
        "Food_sugar_box",
        "Food_gelatin_box",
        "Food_pudding_box",
        "Food_tuna_fish_can",
        "Food_banana",
        "task_d_toy_airplane"
    };

    

    int nextState = 0;
    bool fail = false;
    bool success = false;
    int attemps_object = 0;
    std::string loc = "tall_table";
    std::vector<vision_msgs::VisionObject> recoObj;
    sensor_msgs::Image image;
    int index;
    bool la = false;
    bool ra = false;
    bool drop = false;

    ros::topic::waitForMessage<rosgraph_msgs::Clock>("/clock");

    while(!ros::Time::isValid()){
        ros::spinOnce();
        loop.sleep();
    }

    ros::Duration d(600.0); // 15 min
    ros::Time begin = ros::Time::now();

    while(ros::ok() && !fail && !success && ros::Time::now() - begin <= d){
    
        switch(state){
        	//initial state
            case SM_INIT:
        		std::cout << "State machine: SM_INIT" << std::endl;	
                JustinaManip::torsoGoTo(0.0, 0.0, 0.0, 6000);
        		state = SM_NAVIGATE_TO_TALL_TABLE;
            break;
            //move to the objects initial position
        	case SM_NAVIGATE_TO_TALL_TABLE:
        		//Go to location
        		std::cout << "State machine: SM_NAVIGATE_TO_TALL_TABLE" << std::endl;
			    //go to tall_table because is the easiest path
                JustinaNavigation::moveDist(-0.28, 4000);
                JustinaNavigation::moveDistAngle(0, 1.5707,4000);
                JustinaNavigation::moveDist(0.9, 4000);
                
                state = SM_ALIGN_TABLE;
			    
        	    break;	
            //move to the long table for second try
            case SM_NAVIGATE_TO_LONG_TABLE:
        		//Go to location
        		std::cout << "State machine: SM_NAVIGATE_TO_LONG_TABLE" << std::endl;
			    //go to tall_table because is the easiest path
                loc="long_table";
                if(!JustinaNavigation::getClose(loc, 120000)){
                    std::cout << "Cannot move to "<<loc << std::endl;
                }
                //manual align
                JustinaManip::hdGoTo(0.0, -0.8, 4000);
                JustinaNavigation::moveDist(0.3, 4000);
                JustinaNavigation::moveDistAngle(0, 1.5707,4000);
                state = SM_ALIGN_TABLE;
			    
        	    break;	
            //Align and get close to the table
    		case SM_ALIGN_TABLE:
    			std::cout << "State machine: SM_ALIGN_TABLE" << std::endl;
                std::cout << "I aling with the tall table" << std::endl;
    			JustinaManip::torsoGoTo(0.0, 0.0, 0.0, 6000);
        		//table_aligned = JustinaTasks::alignWithTable(0.2);
                JustinaNavigation::moveDist(0.15, 4000);
                state = SM_DETECT_OBJECT;
                
    			break;
                
			//search for an object for each arm
            case SM_DETECT_OBJECT:
    			std::cout << "State machine: SM_DETECT_OBJECT" << std::endl;
                //try to find an object     
                JustinaManip::torsoGoTo(0.0, 0.0, 0.0, 6000);
                JustinaManip::hdGoTo(0.0, -0.9, 3000);
                if(JustinaVision::detectAllObjectsVot(recoObj, image, 5)){
                    for(int j = 0; j < recoObj.size() ; j++){
                        obj_allowed=true;
                        //check obj_blacklist
                        for( int k=0; k< obj_blacklist.size(); k++){
                            //if not allow 
                            if(recoObj[j].id.compare(obj_blacklist[k])==0){
                                obj_allowed=false;
                                break;
                            }
                        }

                        if(obj_allowed){
                            // id.rfind("",0) search by prefix
                            //each category
                            if (recoObj[j].id.rfind("Food", 0) == 0){
                                obj_type=Food;
                            }else if (recoObj[j].id.rfind("kitchen", 0) == 0){
                                obj_type=Kitchen;
                            }else if (recoObj[j].id.rfind("shape", 0) == 0){
                                obj_type=Shape;
                            }else if (recoObj[j].id.rfind("tool", 0) == 0){
                                obj_type=Tool;
                            }else if (recoObj[j].id.rfind("task", 0) == 0){
                                obj_type=Task;
                            }else{
                                obj_type=None;
                            }
                            //select arm
                            //left
                            if(recoObj[j].pose.position.y > 0 && left_obj_index<0){
                                left_obj_type=obj_type;
                                left_obj_index=j;
                                state = SM_GRASP_OBJECT;
                                std::cout<< "Selecting "<<recoObj[j].id<<" with left arm"<<std::endl;
                            }
                            //right
                            else if(right_obj_index<0){
                                right_obj_type=obj_type;
                                right_obj_index=j;
                                state = SM_GRASP_OBJECT;
                                relative_right_pose[0]=recoObj[right_obj_index].pose.position.x;
                                relative_right_pose[1]=recoObj[right_obj_index].pose.position.y;
                                relative_right_pose[2]=recoObj[right_obj_index].pose.position.z;
                                JustinaTools::transformPose("base_link",relative_right_pose,"map",world_right_pose);
                                std::cout<< "Selecting "<<recoObj[j].id<<" with right arm"<<std::endl;
                            }
                            
                        }
                    }
                } 
                if(right_obj_index<0 && left_obj_index<0){
                    if(tall_table_attempts <4){
                        tall_table_attempts++;
                        JustinaNavigation::moveDist(-0.05, 4000);
                        state = SM_DETECT_OBJECT;
                    }else{
                        state=SM_NAVIGATE_TO_LONG_TABLE;
                    }
                }
    		    break;
            //if and object has been detected try to grasp it
            case SM_GRASP_OBJECT:
			    std::cout << "State machine: SM_GRASP_OBJECT" << std::endl;

                //try to grasp objects with selected arm
                JustinaManip::torsoGoTo(0,0,0,5000);
                if(left_obj_index>=0){
                    JustinaTasks::graspObject(recoObj[left_obj_index].pose.position.x, recoObj[left_obj_index].pose.position.y, recoObj[left_obj_index].pose.position.z, true, "", true);
                    state = SM_LEFT_NAVIGATE_LOCATION;               		
                }
                JustinaManip::torsoGoTo(0,0,0,5000);
                if(right_obj_index>=0){
                    JustinaTools::transformPose("map",world_right_pose,"base_link",relative_right_pose);
                    JustinaTasks::graspObject(relative_right_pose[0],relative_right_pose[1],relative_right_pose[2], false, "", true);
                    if (state != SM_LEFT_NAVIGATE_LOCATION){
                        state = SM_RIGHT_NAVIGATE_LOCATION;  
                    }
                }
                //if no obj grasped

                /*
				if (state!= SM_RIGHT_NAVIGATE_LOCATION && state!= SM_LEFT_NAVIGATE_LOCATION ){
                    state= SM_DETECT_OBJECT;		        
        		    std::cout << "No object grasped" << std::endl;
                }*/

			    break;
            //go to deliver zone
            case SM_LEFT_NAVIGATE_LOCATION:
        		//Go to location
        		std::cout << "State machine: SM_LEFT_NAVIGATE_LOCATION" << std::endl;
                //std::string target = get_location(right_obj_type);
                std::cout << "Moving to " << get_location(left_obj_type)<< std::endl;
                if(!JustinaNavigation::getClose(get_location(left_obj_type), 120000)){
                    std::cout << "Cannot move to "<<get_location(left_obj_type) << std::endl;
                }
                JustinaNavigation::moveDist(0.4, 4000);
                state = SM_LEFT_DELIVER_OBJECT;
        	    break;
            case SM_RIGHT_NAVIGATE_LOCATION:
        		//Go to location
        		std::cout << "State machine: SM_RIGHT_NAVIGATE_LOCATION" << std::endl;
                //std::string target = get_location(right_obj_type);
                std::cout << "Moving to " << get_location(right_obj_type)<< std::endl;
                if(!JustinaNavigation::getClose(get_location(right_obj_type), 120000)){
                    std::cout << "Cannot move to "<<get_location(right_obj_type) << std::endl;
                }
                JustinaNavigation::moveDist(0.4, 4000);
                state = SM_RIGHT_DELIVER_OBJECT;
        	    break;

            case SM_RIGHT_DELIVER_OBJECT:
				std::cout << "State machine: SM_RIGHT_DELIVER_OBJECT" << std::endl;
				
                if(right_obj_index>=0){
                	JustinaManip::raGoTo("take", 3000);
                	JustinaTasks::dropObject("", false, 10000);
                    right_obj_index=-1;
                    right_obj_type=None;
                    left_obj_type=None;
            	}
                JustinaManip::hdGoTo(0.0, 0.0, 6000);
                state=SM_NAVIGATE_TO_LONG_TABLE;
                break;
            
            case SM_LEFT_DELIVER_OBJECT:
				std::cout << "State machine: SM_LEFT_DELIVER_OBJECT" << std::endl;
				
            	if(left_obj_index>=0){
            		JustinaManip::laGoTo("take", 3000);
            		JustinaTasks::dropObject("", true, 10000);
                    left_obj_index=-1;
            	}
     
                JustinaManip::hdGoTo(0.0, 0.0, 6000);
                //if is same delivery place
                if(right_obj_index>=0){
                    if(get_location(right_obj_type)==get_location(left_obj_type)){
                        state = SM_RIGHT_DELIVER_OBJECT; 
                    }else{
                        state = SM_RIGHT_NAVIGATE_LOCATION; 
                    }
                }else{
                    state=SM_NAVIGATE_TO_LONG_TABLE;
                }
		        break;
                
            case SM_FINAL_STATE:
        		//Final state
        		std::cout << "State machine: SM_FINAL_STATE" << std::endl;	

                //JustinaNavigation::getClose("initial_point", 120000);
                //JustinaHRI::waitAfterSay("I have finished test",4000);
			    std::cout << "I have finished task 1" << std::endl;	
        		return 0;
        }
        ros::spinOnce();
        loop.sleep();
    }
    std::cout << "TIME IS OVER" << std::endl;
    return 0;
}
