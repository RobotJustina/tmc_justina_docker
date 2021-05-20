#!/bin/sh

sudo cp /opt/ros/melodic/lib/shared_libs/libSDL-1.2.so.0.11.4 /usr/lib/x86_64-linux-gnu/libSDL-1.2.so.0
sudo cp /opt/ros/melodic/lib/shared_libs/libSDL_image-1.2.so.0.8.4 /usr/lib/x86_64-linux-gnu/libSDL_image-1.2.so.0
sudo cp /opt/ros/melodic/lib/shared_libs/libpulse-simple.so.0.1.1 /usr/lib/x86_64-linux-gnu/libpulse-simple.so.0
sudo cp /opt/ros/melodic/lib/shared_libs/libpulse.so.0.20.2 /usr/lib/x86_64-linux-gnu/libpulse.so.0
sudo cp /opt/ros/melodic/lib/shared_libs/libsndfile.so.1.0.28 /usr/lib/x86_64-linux-gnu/libsndfile.so.1
sudo cp /opt/ros/melodic/lib/shared_libs/libasyncns.so.0.3.1 /usr/lib/x86_64-linux-gnu/libasyncns.so.0

sudo cp /opt/ros/melodic/lib/shared_libs/libwrap.so.0.7.6 /lib/x86_64-linux-gnu/libwrap.so.0

sudo mkdir /usr/lib/x86_64-linux-gnu/pulseaudio
sudo cp /opt/ros/melodic/lib/shared_libs/libpulsecommon-11.1.so /usr/lib/x86_64-linux-gnu/pulseaudio/libpulsecommon-11.1.so


sudo cp /opt/ros/melodic/lib/shared_libs/libpcl_common.so.1.8.1 /usr/lib/x86_64-linux-gnu/libpcl_common.so.1.8
sudo cp /opt/ros/melodic/lib/shared_libs/* /opt/ros/melodic/lib
