from launch import LaunchDescription
from launch_ros.actions import Node
import os
from ament_index_python.packages import get_package_share_directory

def generate_launch_description():
    package_name = 'articubot_one'
    mux_params_file = os.path.join(get_package_share_directory(package_name), 'config', 'twist_mux.yaml')
    
    return LaunchDescription([
        # Node for twist_mux
        Node(
            package='twist_mux',
            executable='twist_mux',
            name='twist_mux',
            output='screen',
            parameters=[mux_params_file],
            remappings=[('/cmd_vel_out', '/cmd_vel')]
        ),
        
        # Node for teleop_twist_joy
        Node(
            package='teleop_twist_joy',
            executable='teleop_node',
            name='teleop_node',
            output='screen',
            parameters=[{
                'axis_linear.x': 1,
                'scale_linear.x': 0.5,
                'scale_linear_turbo.x': 0.8,
                'use_sim_time': False,
                'axis_angular.yaw': 3,
                'scale_angular.yaw': 2.0,
                'scale_angular_turbo.yaw': 10.0,
                'enable_button': 5,
                'enable_turbo_button': 4,
                'require_enable_button': True,
            }],
            remappings=[('/cmd_vel', '/cmd_vel_joy')]
        ),
        
        # Node for joy (joystick input)
        Node(
            package='joy',
            executable='joy_node',
            name='joy_node',
            output='screen',
            parameters=[{
                'deadzone': 0.1,
                'use_sim_time': False,
            }]
        ),
    ])
