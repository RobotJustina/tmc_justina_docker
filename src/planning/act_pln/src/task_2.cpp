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
	SM_NAVIGATE_TO_SECOND_ROOM,
    SM_NAVIGATE_TO_SHELF,
	SM_ALIGN_SHELF,
	SM_DETECT_OBJECT,
	SM_GRASP_OBJECT,
    SM_NAVIGATE_LOCATION,
    SM_DELIVER_OBJECT,
	SM_FINAL_STATE
};
//find if is suffix
bool endsWith (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}




//-------arms
std::string desired_obj="";
std::string desired_person="";
int obj_index=-1;
bool left_arm=true;
bool left_person=true;

//callback with desired_obj
void desiredObjCallback(const std_msgs::String::ConstPtr& msg){
    std::string message=msg->data.c_str();
    std::size_t found = message.find_last_of(" ");
    //check if left person is the correct
    left_arm=message.substr(found+1).compare("left")==0;
    //split object name
    found = message.find(" ");
    desired_obj=message.substr(0,found);
    //print values
    std::cout<<"Desired object  "<< desired_obj <<std::endl;
    std::cout<<"Desired person left = "<< left_arm <<std::endl;
}

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
    int attemps_object = 0;
    std::string loc = "second_room";
    std::vector<vision_msgs::VisionObject> recoObj;
    sensor_msgs::Image image;

    ros::topic::waitForMessage<rosgraph_msgs::Clock>("/clock");
    ros::Subscriber obj_topic= n.subscribe("/message", 1, desiredObjCallback); 

    //object blacklist 
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
    

    while(!ros::Time::isValid()){
        ros::spinOnce();
        loop.sleep();
    }

    ros::Duration d(300.0); // 5 min
    ros::Time begin = ros::Time::now();

    while(ros::ok() && !fail && !success && ros::Time::now() - begin <= d){
    
        switch(state){
        	//initial state
            case SM_INIT:
        		std::cout << "State machine: SM_INIT" << std::endl;	
        		state = SM_NAVIGATE_TO_SHELF;
            break;
            //move to the objects initial position
        	case SM_NAVIGATE_TO_SECOND_ROOM:
        		//Go to location
        		std::cout << "State machine: SM_NAVIGATE_TO_SECOND_ROOM" << std::endl;
			    //go to tall_table because is the easiest path
                if(!JustinaNavigation::getClose(loc, 120000)){
                    std::cout << "Cannot move to "<<loc << std::endl;
                }                
                state = SM_NAVIGATE_TO_SHELF;
			    
        	    break;	
            //Move to the shelf
            case SM_NAVIGATE_TO_SHELF:
                std::cout << "State machine: SM_NAVIGATE_TO_SHELF" << std::endl;
			    //move to shelf
                loc="shelf";
                if(!JustinaNavigation::getClose(loc, 120000)){
                    std::cout << "Cannot move to "<<loc << std::endl;
                } 
                
                  
                //JustinaNavigation::moveDist(0.3, 4000);
                state=SM_DETECT_OBJECT;
                break;

            //Align and get close to the table
    		case SM_ALIGN_SHELF:
    			std::cout << "State machine: SM_ALIGN_SHELF" << std::endl;
                std::cout << "I aling with the "<<loc << std::endl;
    			JustinaManip::torsoGoTo(0.0, 0.0, 0.0, 6000);
        		table_aligned = JustinaTasks::alignWithTable(0.2);
                JustinaNavigation::moveDist(0.3, 4000);
                attemps_object++;
                if(attemps_object > 2){
                    state = SM_NAVIGATE_TO_SHELF;
                    attemps_object = 0;
                }
                else{
        		    state = SM_DETECT_OBJECT;
                }
    			break;
                
			//search for an object for each arm
            case SM_DETECT_OBJECT:
    			std::cout << "State machine: SM_DETECT_OBJECT" << std::endl;
                //pan head 
                JustinaManip::hdGoTo(0.0, -0.8, 3000);

                //try to find an object     
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
                            // id.rfind("",0) search by suffix
                            //each category
                            if (endsWith(recoObj[j].id,desired_obj)){
                                obj_index=j;
                                std::cout << "--Desired object "<<desired_obj<<" found." << std::endl;
                                break;
                            }else if (obj_index<0 || recoObj[j].pose.position.x<recoObj[obj_index].pose.position.x){
                                obj_index=j;
                            }
                        }
                                              
                    }
                    //if an object was selected
                    if(obj_index>=0){
                        //select arm
                        //left
                        if(recoObj[obj_index].pose.position.y > 0){
                            left_arm=true;
                            std::cout<< "Selecting "<<recoObj[obj_index].id<<" with left arm"<<std::endl;
                        }
                        //right
                        else {
                            left_arm=false;
                            std::cout<< "Selecting "<<recoObj[obj_index].id<<" with right arm"<<std::endl;
                        }
                        state = SM_GRASP_OBJECT;
                    }
                } else{
                    JustinaNavigation::moveDist(-0.05, 4000);
                    state = SM_DETECT_OBJECT;
                }
                break;
            //if and object has been detected try to grasp it
            case SM_GRASP_OBJECT:
			    std::cout << "State machine: SM_GRASP_OBJECT" << std::endl;

                //try to grasp objects with selected arm
                JustinaManip::torsoGoTo(0,0,0,5000);
                JustinaTasks::graspObject(recoObj[obj_index].pose.position.x, recoObj[obj_index].pose.position.y, recoObj[obj_index].pose.position.z, left_arm, "", true);
                
                //if no obj grasped

                /*
				if (state!= SM_RIGHT_NAVIGATE_LOCATION && state!= SM_LEFT_NAVIGATE_LOCATION ){
                    state= SM_DETECT_OBJECT;		        
        		    std::cout << "No object grasped" << std::endl;
                }*/
                state=SM_NAVIGATE_LOCATION;
			    break;

            //go to deliver zone
            case SM_NAVIGATE_LOCATION:
                
                //select location
                desired_person=left_person?"chair_b":"chair_a";

        		//Go to location
        		std::cout << "State machine: SM_NAVIGATE_LOCATION" << std::endl;
                //std::string target = get_location(right_obj_type);
                std::cout << "Moving to " << desired_person<< std::endl;
                if(!JustinaNavigation::getClose(desired_person ,120000)){
                    std::cout << "Cannot move to "<< desired_person << std::endl;
                }
                JustinaNavigation::moveDist(0.1, 4000);
                state = SM_DELIVER_OBJECT;
        	    break;
            
            case SM_DELIVER_OBJECT:
				std::cout << "State machine: SM_LEFT_DELIVER_OBJECT" << std::endl;
				
            	if(obj_index>=0){
                    if(left_arm)
            		    JustinaManip::laGoTo("take", 3000);
                    else
                    {
                        JustinaManip::raGoTo("take", 3000);
                    }
                    JustinaTasks::dropObject("", left_arm, 10000);
                    obj_index=-1;
            	}
     
                JustinaManip::hdGoTo(0.0, 0.0, 6000);
                state = SM_FINAL_STATE; 
		        break;
                
            case SM_FINAL_STATE:
        		//Final state
        		std::cout << "State machine: SM_FINAL_STATE" << std::endl;	

                //JustinaNavigation::getClose("initial_point", 120000);
                //JustinaHRI::waitAfterSay("I have finished test",4000);
			    std::cout << "I have finished task 2" << std::endl;	
        		return 0;
        }
        ros::spinOnce();
        loop.sleep();
    }
    std::cout << "TIME IS OVER" << std::endl;
    return 0;
}
