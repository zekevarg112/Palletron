import os
from ament_index_python.packages import get_package_share_directory
from launch import LaunchDescription
from launch.actions import IncludeLaunchDescription
from launch.launch_description_sources import PythonLaunchDescriptionSource
from launch_ros.actions import Node

def generate_launch_description():
    package_name = 'articubot_one'  # Confirm this is your package name

    # Include the Velodyne launch file
    velodyne = IncludeLaunchDescription(
        PythonLaunchDescriptionSource([os.path.join(
            get_package_share_directory('velodyne'), 'launch', 'velodyne-all-nodes-VLP16-launch.py'
        )])
    )

    # First instance of pointcloud_to_laserscan node
    pointcloud_to_laserscan_node_long = Node(
        package='pointcloud_to_laserscan',
        executable='pointcloud_to_laserscan_node',
        name='pointcloud_to_laserscan_long',
        parameters=[{
            'range_min': 0.4,
            'range_max': 100.0,
            'scan_time': 0.02,
            'use_sim_time': False,
            'min_height': -0.3,
            'max_height': 1.5,
            'angle_min': -3.1415,
            'angle_max': 3.1415,
            'angle_increment': 0.00349,
            'use_inf': True,
        }],
        remappings=[
            ('cloud_in', '/velodyne_points'),  # Input topic for point cloud data
            ('scan', '/velodyne_scan_long')  # Output topic for the first scan
        ],
        output='screen'
    )

    # Second instance of pointcloud_to_laserscan node with different parameters or remappings
    pointcloud_to_laserscan_node_low = Node(
        package='pointcloud_to_laserscan',
        executable='pointcloud_to_laserscan_node',
        name='pointcloud_to_laserscan_low',
        parameters=[{
            'range_min': 0.4,
            'range_max': 5.0,
            'scan_time': 0.02,
            'use_sim_time': False,
            'min_height': -0.7,  # Adjusted height for the second instance
            'max_height': -0.3,
            'angle_min': -3.1415,
            'angle_max': 3.1415,
            'angle_increment': 0.00349,
            'use_inf': True,
        }],
        remappings=[
            ('cloud_in', '/velodyne_points'),  # Same input topic or different if required
            ('scan', '/velodyne_scan_low')  # Different output topic for the second scan
        ],
        output='screen'
    )

    fusion_node = Node(
        package='articubot_one',
        executable='laser_scan_fusion_node.py',
        name='laser_scan_fusion_node',
        output='screen'
    )

    # Launch both nodes along with Velodyne
    return LaunchDescription([
        velodyne,
        pointcloud_to_laserscan_node_long,
        pointcloud_to_laserscan_node_low,
        fusion_node
    ])
