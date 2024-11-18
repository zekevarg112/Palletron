#!/bin/bash
# Source ROS 2 setup script
source /opt/ros/humble/setup.bash
# Source your workspace (if applicable)
source ~/Palletron/ROS/install/setup.bash
# Run the launch file
ros2 launch motor_service raspberry_launch.py