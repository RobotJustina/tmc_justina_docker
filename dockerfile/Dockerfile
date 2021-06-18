FROM ros:melodic-ros-base

SHELL ["/bin/bash", "-c"]

ENV DEBIAN_FRONTEND=noninteractive
RUN apt-get update && \
    apt-get install -y curl wget apt-transport-https python-pip && \
    apt-get clean

RUN sh -c 'echo "deb http://packages.osrfoundation.org/gazebo/ubuntu-stable `lsb_release -cs` main" > /etc/apt/sources.list.d/gazebo-stable.list' && \
    curl -L http://packages.osrfoundation.org/gazebo.key | apt-key add -

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
    ros-melodic-gazebo-ros ros-melodic-gazebo-plugins ros-melodic-gazebo-ros-control libgazebo9-dev libignition-transport4-dev libpoco-dev python-scipy libgsl-dev \
    ros-melodic-dwa-local-planner \
    ros-melodic-eigen-conversions \
    ros-melodic-robot-state-publisher \
    ros-melodic-moveit-core \
    ros-melodic-moveit-plugins \
    ros-melodic-moveit-planners-ompl \
    ros-melodic-moveit-ros-planning \
    ros-melodic-moveit-ros-move-group \
    ros-melodic-moveit-ros-manipulation \
    ros-melodic-moveit-simple-controller-manager \
    ros-melodic-urdfdom-py \
    ros-melodic-roslint \
    ros-melodic-joint-state-controller \
    ros-melodic-joint-trajectory-controller \
    ros-melodic-move-base \
    ros-melodic-map-server \
    ros-melodic-xacro \
    ros-melodic-joint-state-publisher \
    liburdfdom-tools \
    ros-melodic-image-proc \
    ros-melodic-depth-image-proc \
    ros-melodic-effort-controllers \
    ros-melodic-ros-controllers \
    ros-melodic-pcl-ros \
    ros-melodic-tf-conversions \
    ros-melodic-moveit-ros-perception && \
    pip install -U --ignore-installed pyassimp supervisor supervisor_twiddler && \
    apt-get autoremove -y && \
    apt-get clean

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
    ros-melodic-tf2-bullet \
    ros-melodic-interactive-markers \
    ros-melodic-sound-play \
    ros-melodic-rviz && \
    apt-get autoremove -y && \
    apt-get clean

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
    ros-melodic-joy \
    ros-melodic-pointcloud-to-laserscan\
    ros-melodic-amcl \
    ros-melodic-map-server \
    ros-melodic-gmapping && \
    apt-get autoremove -y && \
    apt-get clean

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
    qt4-default \
    unzip && \
    apt-get autoremove -y && \
    apt-get clean

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
    ros-melodic-opencv* \
    libpcl-dev && \
    apt-get autoremove -y && \
    apt-get clean

RUN wget https://sourceforge.net/projects/opencvlibrary/files/opencv-unix/3.3.1/opencv-3.3.1.zip && unzip opencv-3.3.1.zip && wget https://github.com/opencv/opencv_contrib/archive/3.3.1.zip && mv 3.3.1.zip opencv_contrib-3.3.1.zip && unzip opencv_contrib-3.3.1.zip && mkdir /opencv-3.3.1/build && cd /opencv-3.3.1/build && cmake -D WITH_TBB=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D WITH_V4L=ON -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=OFF -D BUILD_EXAMPLES=OFF -D WITH_QT=ON -D WITH_OPEPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=ON -D WITH_QT=ON -D WITH_OPENGL=ON -D WITH_VTK=ON -D WITH_OPENNI=OFF -D WITH_OPENCL=OFF -D CMAKE_BUILD_TYPE=RELEASE FORCE_VTK=ON -D WITH_CUBLAS=ON -D -DBUILD_opencv_cudalegacy=OFF CUDA_NVCC_FLAGS="-D_FORCE_INLINES" -D WITH_GDAL=ON -D WITH_XINE=ON -D BUILD_EXAMPLES=OFF -D OPENCV_EXTRA_MODULES_PATH=/opencv_contrib-3.3.1/modules .. && make -j4 && make install && touch /etc/ld.so.conf.d/opencv.conf && /bin/su -c "echo '/usr/local/lib' >> /etc/ld.so.conf.d/opencv.conf" && ldconfig && rm /opencv-3.3.1.zip && rm /opencv_contrib-3.3.1.zip && rm /opencv_contrib-3.3.1 -R && rm /opencv-3.3.1 -R && cp /usr/local/lib/libopencv_* /usr/lib/x86_64-linux-gnu/

#RUN mkdir /opencv-3.3.1 && mkdir /opencv_contrib-3.3.1
#ADD opencv-3.3.1/ /opencv-3.3.1
#ADD opencv_contrib-3.3.1/ /opencv_contrib-3.3.1
#RUN mkdir /opencv-3.3.1/build && cd /opencv-3.3.1/build && \
#    cmake -D WITH_TBB=ON -D BUILD_NEW_PYTHON_SUPPORT=ON -D WITH_V4L=ON -D INSTALL_C_EXAMPLES=ON -D INSTALL_PYTHON_EXAMPLES=ON -D BUILD_EXAMPLES=ON -D WITH_QT=ON -D WITH_OPENGL=ON -D WITH_VTK=ON -D WITH_OPENNI=ON -D WITH_OPENCL=OFF -D OPENCV_EXTRA_MODULES_PATH=../../opencv_contrib-3.3.1/modules .. && \
#    make -j4 && make install && touch /etc/ld.so.conf.d/opencv.conf && \
#    /bin/su -c "echo '/usr/local/lib' >> /etc/ld.so.conf.d/opencv.conf" && \
#    ldconfig
#RUN rm -rf /opencv-3.3.1 /opencv_contrib-3.3.1

#RUN mkdir /OpenNI
#ADD OpenNI /OpenNI
#RUN cd /OpenNI/Platform/Linux/CreateRedist/ && ./RedistMaker
#RUN cd /OpenNI/Platform/Linux/Redist/OpenNI-Bin-Dev-Linux-x64-v1.5.8.5/ && ./install.sh
#RUN rm -rf /OpenNI

RUN apt-get update && \
    apt-get upgrade -y && \
    apt-get install --no-install-recommends -y \
    ros-melodic-opencv* \
    libpcl-dev && \
    apt-get autoremove -y && \
    apt-get clean

RUN  mkdir /catkin_ws 

ADD catkin_ws/src /catkin_ws/src

RUN cd /catkin_ws/src && source /opt/ros/$ROS_DISTRO/setup.bash && catkin_init_workspace || true

RUN cd /catkin_ws && source /opt/ros/$ROS_DISTRO/setup.bash && catkin_make install -DCMAKE_BUILD_TYPE=Release -DCMAKE_INSTALL_PREFIX=/opt/ros/$ROS_DISTRO -DCATKIN_ENABLE_TESTING=0

RUN cd /opt/ros/melodic/lib && mkdir shared_libs
RUN cp /opt/ros/melodic/lib/libpcl_ros_tf.so /opt/ros/melodic/lib/shared_libs && \
    cp /opt/ros/melodic/lib/libgrid_utils.so /opt/ros/melodic/lib/shared_libs && \
    cp /opt/ros/melodic/lib/libmap_server_image_loader.so /opt/ros/melodic/lib/shared_libs && \
    cp /opt/ros/melodic/lib/libamcl_sensors.so /opt/ros/melodic/lib/shared_libs && \
    cp /opt/ros/melodic/lib/libamcl_map.so /opt/ros/melodic/lib/shared_libs && \
    cp /opt/ros/melodic/lib/libamcl_pf.so /opt/ros/melodic/lib/shared_libs && \
    cp /usr/lib/x86_64-linux-gnu/libSDL-1.2.so.0.11.4 /opt/ros/melodic/lib/shared_libs && \
    cp /usr/lib/x86_64-linux-gnu/libSDL_image-1.2.so.0.8.4 /opt/ros/melodic/lib/shared_libs && \
    cp /usr/lib/x86_64-linux-gnu/libpulse-simple.so.0.1.1 /opt/ros/melodic/lib/shared_libs && \
    cp /usr/lib/x86_64-linux-gnu/libpulse.so.0.20.2 /opt/ros/melodic/lib/shared_libs && \
    cp /usr/lib/x86_64-linux-gnu/libsndfile.so.1.0.28 /opt/ros/melodic/lib/shared_libs && \
    cp /usr/lib/x86_64-linux-gnu/libasyncns.so.0.3.1 /opt/ros/melodic/lib/shared_libs && \
    cp /lib/x86_64-linux-gnu/libwrap.so.0.7.6 /opt/ros/melodic/lib/shared_libs && \
    cp /usr/lib/x86_64-linux-gnu/pulseaudio/libpulsecommon-11.1.so /opt/ros/melodic/lib/shared_libs && \
    cp /usr/lib/x86_64-linux-gnu/libpcl_common.so.1.8.1 /opt/ros/melodic/lib/shared_libs && \
    cp /opt/ros/melodic/lib/libjustina_tools.so /opt/ros/melodic/lib/shared_libs

#RUN cd /opt/ros/melodic/lib && mkdir opencv_libs
#RUN cp /usr/local/lib/libopencv_viz.so.3.3.1 /opt/ros/melodic/lib/opencv_libs && \
#    cp /usr/local/lib/libopencv_videoio.so.3.3.1 /opt/ros/melodic/lib/opencv_libs && \
#    cp /usr/local/lib/libopencv_highgui.so.3.3.1 /opt/ros/melodic/lib/opencv_libs && \
#    cp /usr/local/lib/libopencv_imgproc.so.3.3.1 /opt/ros/melodic/lib/opencv_libs && \
#    cp /usr/local/lib/libopencv_core.so.3.3.1 /opt/ros/melodic/lib/opencv_libs && \
#    cp /usr/local/lib/libopencv_imgcodecs.so.3.3.1 /opt/ros/melodic/lib/opencv_libs

RUN cp /usr/lib/x86_64-linux-gnu/libpcl* /usr/local/lib -P && \
    cp /usr/lib/libOpenNI2.so* /usr/local/lib/ -P && \
    cp /usr/lib/libOpenNI.so* /usr/local/lib/ -P

ADD entrypoint.sh /entrypoint.sh

ENTRYPOINT ["/entrypoint.sh"]

RUN source /opt/ros/$ROS_DISTRO/setup.bash && rosrun tmc_gazebo_task_evaluators setup_score_widget

ADD supervisord.conf /etc/supervisor/supervisord.conf

VOLUME ["/opt/ros/melodic/share/hsrb_meshes", "/opt/ros/melodic/share/justina_plugins", "/opt/ros/melodic/share/tmc_wrs_gazebo_worlds", "/opt/ros/melodic/share/gazebo_ros", "/opt/ros/melodic/lib/gazebo_ros", "/opt/ros/melodic/lib/python2.7/dist-packages/gazebo_ros", "/opt/ros/melodic/lib/python2.7/dist-packages/gazebo_msgs", "/opt/ros/melodic/share/tmc_control_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/tmc_control_msgs", "/opt/ros/melodic/include/tmc_control_msgs", "/opt/ros/melodic/lib/justina_tools /opt/ros/melodic/share/justina_tools", "/opt/ros/melodic/include/justina_tools", "/opt/ros/melodic/include/manip_msgs", "/opt/ros/melodic/share/manip_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/manip_msgs", "/opt/ros/melodic/include/navig_msgs", "/opt/ros/melodic/share/navig_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/navig_msgs", "/opt/ros/melodic/include/vision_msgs", "/opt/ros/melodic/share/vision_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/vision_msgs", "/opt/ros/melodic/include/visualization_msgs", "/opt/ros/melodic/share/visualization_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/visualization_msgs", "/opt/ros/melodic/include/knowledge_msgs", "/opt/ros/melodic/share/knowledge_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/knowledge_msgs", "/opt/ros/melodic/include/env_msgs", "/opt/ros/melodic/share/env_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/env_msgs", "/opt/ros/melodic/lib/kinect_man", "/opt/ros/melodic/share/kinect_man", "/opt/ros/melodic/lib/gmapping /opt/ros/melodic/share/gmapping", "/opt/ros/melodic/lib/joy", "/opt/ros/melodic/share/joy", "/opt/ros/melodic/include/bbros_bridge", "/opt/ros/melodic/share/bbros_bridge", "/opt/ros/melodic/lib/python2.7/dist-packages/bbros_bridge", "/opt/ros/melodic/lib/head", "/opt/ros/melodic/share/head", "/opt/ros/melodic/lib/arms", "/opt/ros/melodic/share/arms", "/opt/ros/melodic/lib/laser_manager", "/opt/ros/melodic/share/laser_manager", "/opt/ros/melodic/lib/shared_libs", "/opt/ros/melodic/share/obj_reco", "/opt/ros/melodic/lib/obj_reco", "/usr/local/lib"]

VOLUME ["/opt/ros/melodic/share/tmc_gazebo_task_evaluators", "/opt/ros/melodic/lib/tmc_gazebo_task_evaluators"]

VOLUME ["/opt/ros/melodic/include/hri_msgs", "/opt/ros/melodic/share/hri_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/hri_msgs"]

VOLUME ["/opt/ros/melodic/lib/pointcloud_to_laserscan", "/opt/ros/melodic/share/pointcloud_to_laserscan", "/opt/ros/melodic/lib/laser_mixer", "/opt/ros/melodic/share/laser_mixer", "/opt/ros/melodic/lib/jrviz", "/opt/ros/melodic/share/jrviz", "/opt/ros/melodic/lib/map_server", "/opt/ros/melodic/share/map_server", "/opt/ros/melodic/lib/amcl", "/opt/ros/melodic/share/amcl"]

VOLUME ["/opt/ros/melodic/share/obs_detect", "/opt/ros/melodic/lib/obs_detect", "/opt/ros/melodic/share/mvn_pln", "/opt/ros/melodic/lib/mvn_pln"]

VOLUME ["/opt/ros/melodic/share/pcl_conversions", "/opt/ros/melodic/include/pcl_conversions", "/opt/ros/melodic/lib/python2.7/dist-packages/pcl_conversions", "/usr/include/pcl-1.8 /usr/include/ni", "/usr/include/openni2", "/usr/include/vtk-6.3", "/usr/include/freetype2", "/usr/include/hdf5", "/usr/include/libxml2", "/usr/include/jsoncpp", "/usr/include/tcl", "/opt/ros/melodic/share/pcl_msgs", "/opt/ros/melodic/include/pcl_msgs", "/opt/ros/melodic/lib/python2.7/dist-packages/pcl_msgs", "/opt/ros/melodic/share/pcl_ros", "/opt/ros/melodic/lib/pcl_ros", "/opt/ros/melodic/include/pcl_ros", "/opt/ros/melodic/lib/python2.7/dist-packages/pcl_ros"]

#VOLUME ["/usr/lib/libOpenNI2.so", "/usr/lib/libOpenNI2.so.0"]

#VOLUME [ /opt/ros/melodic/share/obj_reco /opt/ros/melodic/lib/obj_reco ]

CMD ["/usr/local/bin/supervisord", "-n", "-c", "/etc/supervisor/supervisord.conf"]
