import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node

def generate_launch_description():

    actuator_node = Node(
        package='actuator_control',
        executable='actuator_node',
        name='actuator_node',
        output='screen',
        parameters=[{'use_sim_time': False}]
    )
    control_node = Node(
        package='actuator_control',
        executable='linear_actuator_controller_irl_node',
        name='linear_actuator_controller_irl_node',
        output='screen',
        parameters=[{'use_sim_time': False}]
    )


    # Launch both nodes along with Velodyne
    return LaunchDescription([
        actuator_node,
        control_node
    ])
