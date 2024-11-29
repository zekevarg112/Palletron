from launch import LaunchDescription
from launch_ros.actions import Node
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from ament_index_python.packages import get_package_share_directory
import os
def generate_launch_description():
   
   service_node = Node(
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
   )
   actuator_node = Node(
      package="actuator_control",
      executable="actuator_node",
      name="actuator_node"
   )
   
   lidar_launch_path = os.path.join(
        get_package_share_directory('motor_service'),
        'launch',
        'lidar.launch.py'
    )
   lidar_stuff = IncludeLaunchDescription(
            PythonLaunchDescriptionSource(lidar_launch_path)
        )
#    Node(
#        package='articubot_one',
#        executable='linear_actuator_controller_irl_node',
#        name='linear_actuator_controller_irl_node',
#        output='screen',
#        parameters=[{'use_sim_time': False}]
#    ),
#    Node(
#        package='motor_service',
#        executable='failsafe_node',
#        name='failsafe_node',
#        output='screen',
#        parameters=[{'use_sim_time': False}]
#    )
   return LaunchDescription([
      service_node,
      actuator_node,
      lidar_stuff
   ])