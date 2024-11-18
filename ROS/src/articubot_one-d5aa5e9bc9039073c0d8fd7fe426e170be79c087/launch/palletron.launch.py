import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node
from launch.actions import TimerAction
import xacro

def generate_launch_description():

    package_name='articubot_one' #<--- CHANGE ME

    # Process the URDF file
    pkg_path = os.path.join(get_package_share_directory('articubot_one'))
    xacro_file = os.path.join(pkg_path,'description','robot_irl.urdf.xacro')
    robot_description_config = xacro.process_file(xacro_file)
    
    # Create a robot_state_publisher node
    params = {'robot_description': robot_description_config.toxml(), 'use_sim_time': False}

    node_robot_state_publisher = Node(
        package='robot_state_publisher',
        executable='robot_state_publisher',
        output='screen',
        parameters=[params]
    )

    odometry_node = Node(
        package='articubot_one',
        executable='odom_irl_publisher.py', 
        name='odom_publisher',
        output='screen',
        parameters=[{'use_sim_time': False}]
    )
    delayed_odom_node = TimerAction(
        period=3.0,  # 5-second delay to allow for proper startup
        actions=[odometry_node]
    )

                        
     # Include RViz2 node with 'use_sim_time' set to true
    rviz_config_file = os.path.join(
        get_package_share_directory(package_name), 'rviz', 'robot.rviz'
    )
    rviz_node = Node(
        package='rviz2',
        executable='rviz2',
        name='rviz2',
        #arguments=['-d', rviz_config_file],
        parameters=[{'use_sim_time': False}],
        output='screen'
    )

    mapper_params_file = os.path.join(get_package_share_directory(package_name),'config','mapper_params_online_async_irl.yaml')
    slam_toolbox = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('slam_toolbox'), 'launch', 'online_async_launch.py'
        )]),
        launch_arguments={
            'slam_params_file': mapper_params_file,
            'use_sim_time': 'false'
        }.items()
    )

    delayed_slam_node = TimerAction(
        period=5.0,  # 5-second delay to allow for proper startup
        actions=[slam_toolbox]
    )
    joint_state_publisher_node = Node(
        package='articubot_one',
        executable='palletron_joint_state_publisher_node.py',
        name='palletron_joint_state_publisher_node',
        output='screen',
        parameters=[{'use_sim_time': False}]
    )

    palletron_controller_node = Node(
        package='articubot_one',
        executable='palletron_irl_controller_node.py',
        name='palletron_controller_node',
        output='screen',
        parameters=[{'use_sim_time': False}]
    )
    

    # Launch them all!
    return LaunchDescription([
        palletron_controller_node,
        node_robot_state_publisher,
        rviz_node,
        joint_state_publisher_node, 
        odometry_node,
        slam_toolbox,
#        Node(
#        package="motor_service",
#        executable="motor_controller_service",
#        name='motor_controller',
#        parameters=[
#                    {'baud_rate': 115200},
#                    {'handshake_response_byte': 0xA6},
#                    {'timeout': 5.0}, #handshake timeout
#                    {'publish_rate': 40.0},  # Set your desired publish rate here
#                    {'right_motor_topic': 'right_motor_velocity'},  # Custom topic for right motor
#                    {'left_motor_topic': 'left_motor_velocity'},  # Custom topic for left motor
#                    {'encoder_angle_topic': 'encoder_angle'},  # Custom topic for encoder angle
#                ]
#    ),
        #delayed_odom_node,
        #delayed_slam_node
    ])
