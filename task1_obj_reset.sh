rosservice call /gazebo/delete_model "model_name: 'task_item1'" 
rosservice call /gazebo/delete_model "model_name: 'task_item2'" 
rosservice call /gazebo/delete_model "model_name: 'task_item3'" 
rosservice call /gazebo/delete_model "model_name: 'task_item4'" 
rosservice call /gazebo/delete_model "model_name: 'task_item5'" 

roslaunch surge_et_ambula task1_obj_spawn.launch