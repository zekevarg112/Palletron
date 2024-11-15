## Robot Package Template

# Update package list
sudo apt update

# Install Velodyne driver packages
sudo apt install ros-humble-velodyne*

# Install Navigation 2 (nav2) stack
sudo apt install ros-humble-nav2*

# Install ros2_control and related packages
sudo apt install ros-humble-ros2-control ros-humble-ros2-controllers

# Install xacro for generating URDFs
sudo apt install ros-humble-xacro

# Install Gazebo (ROS 2 integration packages)
sudo apt install ros-humble-gazebo-ros-pkgs

# Install SLAM Toolbox
sudo apt install ros-humble-slam-toolbox

# Install pointcloud_to_laserscan for converting point clouds to laser scans
sudo apt install ros-humble-pointcloud-to-laserscan

# Install twist_mux for managing multiple twist command sources
sudo apt install ros-humble-twist-mux