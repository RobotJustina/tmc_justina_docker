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
	SM_ALIGN_TABLE,
	SM_DETECT_OBJECT,
    SM_NO_DETECT_OBJECT,
	SM_HANDLER,
	SM_GRASP_OBJECT,
	SM_DELIVER_OBJECT,
	SM_REPEAT_TASK,
	SM_FINAL_STATE
};

SMState state = SM_INIT;
bool objectDetected = false;

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

    while(ros::ok() && !fail && !success){
    
        switch(state){
        	case SM_INIT:
        		std::cout << "State machine: SM_INIT" << std::endl;	
        		state = SM_NAVIGATE_TO_INSPECTION;
            break;
        	case SM_NAVIGATE_TO_INSPECTION:
        		//Go to location
        		std::cout << "State machine: SM_NAVIGATE_TO_INSPECTION" << std::endl;
			    //if(change_loc==0){
                    //ss.str("");
                    //ss << "I am going to the kitchen table";
                    //JustinaHRI::waitAfterSay(ss.str(), 5000);
        			if(!JustinaNavigation::getClose("tall_table", 120000)){
        				std::cout << "Cannot move to tall_table" << std::endl;
        			}
                    //ss.str("");
                    //ss << "I have arrived to the kitchen table";
                    //JustinaHRI::waitAfterSay(ss.str(), 5000);
        			state = SM_ALIGN_TABLE;
			    //}
			    /*if(change_loc==1){
                    ss.str("");
                    ss << "I am going to the bed";
                    JustinaHRI::waitAfterSay(ss.str(), 5000);
        			if(!JustinaNavigation::getClose("bed", 120000)){
        				std::cout << "Cannot move to bed" << std::endl;
        			}
                    ss.str("");
                    ss << "I have arrived to the bed";
                    JustinaHRI::waitAfterSay(ss.str(), 5000);
        			state = SM_DELIVER_OBJECT;
			    }*/
        	    break;	
    		case SM_ALIGN_TABLE:
    			std::cout << "State machine: SM_ALIGN_TABLE" << std::endl;
                //JustinaHRI::waitAfterSay("I aling with the kitchen table", 4000);
                std::cout << "I aling with the tall table" << std::endl;
    			JustinaManip::torsoGoTo(0.0, 0.0, 0.0, 6000);
        		objectDetected = JustinaTasks::alignWithTable(0.35);
        		//objectDetected = true;
        		state = SM_DETECT_OBJECT;
    			break;
			
            case SM_DETECT_OBJECT:
    			std::cout << "State machine: SM_DETECT_OBJECT" << std::endl;
    			if(objectDetected){
                    //ss.str("");
                    //ss << "I am looking for objects on the table";
		            //Obtiene la lista de objetos a detectar
		            //recoObj = std::vector<vision_msgs::VisionObject>();
		            objectDetected = false;
		            //Detecta los objetos en la mesa
		            if(JustinaVision::detectAllObjectsVot(recoObj, image, 5)){
		                for(int j = 0; j < recoObj.size() && !objectDetected; j++){
		                	// id.compare es la lista de objetos a leer, en este caso es cocacola
		                    if (recoObj[j].id.compare("cubo") == 0){
		                        index = j;
		                        objectDetected = true;
		                    }
		                }
		            } 
		        }   
	        	state = (objectDetected) ? SM_GRASP_OBJECT : SM_NO_DETECT_OBJECT;
    		    break;
        
        }
        ros::spinOnce();
        loop.sleep();
    }
    return 0;
}
