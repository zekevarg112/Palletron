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
                {'publish_rate': 20.0},  # Set your desired publish rate here
                {'right_motor_topic': 'right_motor_velocity_custom'},  # Custom topic for right motor
                {'left_motor_topic': 'left_motor_velocity_custom'},  # Custom topic for left motor
                {'encoder_angle_topic': 'encoder_angle_custom'},  # Custom topic for encoder angle
            ]
    ),
    Node( #cmd_vel to rpm node
       package="motor_service",
       executable="motor_controller_listener",
       name='Listener'
    ),
   ])