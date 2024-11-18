from launch import LaunchDescription
from launch_ros.actions import Node
def generate_launch_description():
   return LaunchDescription([

    Node(
       package="motor_service",
       executable="motor_controller_service",
       name='motor_controller',
       parameters=[
                {'baud_rate': 115200},
                {'handshake_response_byte': 0xA6},
                {'timeout': 2.0}, #handshake timeout
                {'publish_rate': 40.0},  # Set your desired publish rate here
                {'right_motor_topic': 'right_motor_velocity'},  # Custom topic for right motor
                {'left_motor_topic': 'left_motor_velocity'},  # Custom topic for left motor
                {'encoder_angle_topic': 'encoder_angle'},  # Custom topic for encoder angle
            ]
    ),
    Node(
        package='articubot_one',
        executable='linear_actuator_controller_irl_node.py',
        name='linear_actuator_controller_irl_node',
        output='screen',
        parameters=[{'use_sim_time': False}]
    ),
    Node(
        package='motor_service',
        executable='failsafe_node.py',
        name='failsafe_node',
        output='screen',
        parameters=[{'use_sim_time': False}]
    )
   ])