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

int main(int argc, char** argv){
	std::cout << "INITIALIZING ACT_PLN BY MARCOSOFT..." << std::endl; //cout
    ros::init(argc, argv, "act_pln");
    ros::NodeHandle n;
    //Setting nodeHandle
    /*JustinaHardware::setNodeHandle(&n);
    JustinaManip::setNodeHandle(&n);
    JustinaNavigation::setNodeHandle(&n);
    JustinaTasks::setNodeHandle(&n);
    JustinaTools::setNodeHandle(&n);
    JustinaVision::setNodeHandle(&n);*/
    ros::Rate loop(10);

    int nextState = 0;
    bool fail = false;
    bool success = false;

    while(ros::ok() && !fail && !success){
    
        ros::spinOnce();
        loop.sleep();
    }
    return 0;
}
