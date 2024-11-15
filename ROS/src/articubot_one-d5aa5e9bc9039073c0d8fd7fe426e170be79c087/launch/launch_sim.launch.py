import os

from ament_index_python.packages import get_package_share_directory


from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource

from launch_ros.actions import Node
from launch.actions import TimerAction



def generate_launch_description():


    # Include the robot_state_publisher launch file, provided by our own package. Force sim time to be enabled
    # !!! MAKE SURE YOU SET THE PACKAGE NAME CORRECTLY !!!

    package_name='articubot_one' #<--- CHANGE ME

    odometry_node = Node(
        package='articubot_one',
        executable='odom_publisher.py',  # Ensure this matches the executable name (remove ".py")
        name='odom_publisher',
        output='screen',
        parameters=[{'use_sim_time': True}]
    )

    rsp = IncludeLaunchDescription(
                PythonLaunchDescriptionSource([os.path.join(
                    get_package_share_directory(package_name),'launch','rsp.launch.py'
                )]), launch_arguments={'use_sim_time': 'true'}.items()
    )

    gazebo_params_file = os.path.join(get_package_share_directory(package_name),'config','gazebo_params.yaml')
    # Include the Gazebo launch file, provided by the gazebo_ros package
    #gazebo = IncludeLaunchDescription(
    #            PythonLaunchDescriptionSource([os.path.join(
    #                get_package_share_directory('gazebo_ros'), 'launch', 'gazebo.launch.py')]),
    #         )
    gazebo = IncludeLaunchDescription(
                PythonLaunchDescriptionSource([os.path.join(
                    get_package_share_directory('gazebo_ros'), 'launch', 'gazebo.launch.py')]),
                    launch_arguments={'extra_gazebo_args': '--ros-args --params-file ' + gazebo_params_file}.items()
             )

    # Run the spawner node from the gazebo_ros package. The entity name doesn't really matter if you only have a single robot.
    spawn_entity = Node(package='gazebo_ros', executable='spawn_entity.py',
                        arguments=['-topic', 'robot_description',
                                   '-entity', 'my_bot','-z','1.27'],
                        output='screen')
    
    left_wheel_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["steering_wheel_left_controller"],
    )

    right_wheel_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["steering_wheel_right_controller"],
    )

    # Spawner for the linear actuator controller
    linear_actuator_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["linear_actuator_controller"],
    )
    left_actuator_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["fork_left_actuator_controller"],
    )
    right_actuator_controller_spawner = Node(
        package="controller_manager",
        executable="spawner",
        arguments=["fork_right_actuator_controller"],
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
        parameters=[{'use_sim_time': True}],
        output='screen'
    )

    # Add the pointcloud_to_laserscan node to convert 3D LiDAR data into 2D scan
    pointcloud_to_laserscan_node = Node(
        package='pointcloud_to_laserscan',
        executable='pointcloud_to_laserscan_node',
        name='pointcloud_to_laserscan',
        parameters=[{
            'range_min': 0.4,
            'range_max': 50.0,
            'scan_time': 0.02,
            #'queue_size': 10.0,
            'use_sim_time': True,
            'min_height': -0.40,  # Adjust based on the height of the 3D LiDAR
            'max_height': 0.3,  # Slice the point cloud to get the 2D scan
            'angle_min': -3.1415,  # -180 degrees (radians)
            'angle_max': 3.1415,   # +180 degrees (radians)
            'angle_increment': 0.0174533,  # Angular resolution in radians (~1 degree)
            'use_inf': True,
        }],
        remappings=[
            ('cloud_in', '/CloudScan'),  # Replace this with the actual topic for your point cloud from Gazebo
            ('scan', '/scan')  # This is where the 2D scan will be published
        ],
        output='screen'
    )

    joy_params = os.path.join(get_package_share_directory(package_name),'config','xbox.config.yaml')

    joy_node = Node(
            package='joy',
            executable='joy_node',
            parameters=[joy_params, {'use_sim_time': True}],
         )

    teleop_node = Node(
            package='teleop_twist_joy',
            executable='teleop_node',
            name='teleop_node',
            parameters=[joy_params, {'use_sim_time': True}],
            remappings=[('/cmd_vel','/cmd_vel_joy')]
         )

    delayed_robot_node = TimerAction(
        period=0.1,  # 5-second delay to allow for proper startup
        actions=[spawn_entity]
    )

    # Adding delay to ensure Gazebo and other necessary nodes are started before odometry node
    delayed_odometry_node = TimerAction(
        period=5.0,  # 5-second delay to allow for proper startup
        actions=[odometry_node]
    )
    

    mapper_params_file = os.path.join(get_package_share_directory(package_name),'config','mapper_params_online_async.yaml')
    slam_toolbox = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('slam_toolbox'), 'launch', 'online_async_launch.py'
        )]),
        launch_arguments={
            'slam_params_file': mapper_params_file,
            'use_sim_time': 'true'
        }.items()
    )

    delayed_slam_node = TimerAction(
        period=5.0,  # 5-second delay to allow for proper startup
        actions=[slam_toolbox]
    )
    gaz_motor_driver_node = Node(
        package='articubot_one',
        executable='gazebo_motor_driver.py',
        name='gazebo_motor_driver',
        output='screen',
        parameters=[{'use_sim_time': True}]
    )

    palletron_controller_node = Node(
        package='articubot_one',
        executable='palletron_controller_node.py',
        name='palletron_controller_node',
        output='screen',
        parameters=[{'use_sim_time': True}]
    )

    linear_actuator_controller_node = Node(
        package='articubot_one',
        executable='linear_actuator_controller_node.py',
        name='linear_actuator_controller_node',
        output='screen',
        parameters=[{'use_sim_time': True}]
    )
    



    # Launch them all!
    return LaunchDescription([
        gazebo,
        rsp,
        #delayed_robot_node,
        spawn_entity,
        rviz_node,  # Add this line
        left_wheel_controller_spawner,
        right_wheel_controller_spawner,
        linear_actuator_controller_spawner,
        #left_actuator_controller_spawner,
        #right_actuator_controller_spawner,
        pointcloud_to_laserscan_node,
        #joy_node,
        #teleop_node,
        #slam_toolbox,
        gaz_motor_driver_node,
        palletron_controller_node,
        linear_actuator_controller_node,
        delayed_odometry_node,
        delayed_slam_node
    ])
