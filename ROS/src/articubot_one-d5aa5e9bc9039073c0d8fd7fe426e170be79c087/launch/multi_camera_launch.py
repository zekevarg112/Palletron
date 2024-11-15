from launch import LaunchDescription
from launch_ros.actions import Node
from ament_index_python.packages import get_package_share_directory
import os


def generate_launch_description():
    # Define the package and paths to the configuration files
    package_name = 'articubot_one'
    camera0_params_file = os.path.join(get_package_share_directory(package_name), 'config', 'camera0.yaml')
    camera1_params_file = os.path.join(get_package_share_directory(package_name), 'config', 'camera1.yaml')

    # Define the device paths and namespaces for each camera
    camera_configs = [
        {
            'device': '/dev/logitech_cam_0',
            'namespace': 'camera_0',
            'params_file': camera0_params_file
        },
        {
            'device': '/dev/logitech_cam_1',
            'namespace': 'camera_1',
            'params_file': camera1_params_file
        }
    ]

    # Create Node instances for each camera with unique configurations
    nodes = []
    for config in camera_configs:
        node = Node(
            package='v4l2_camera',
            executable='v4l2_camera_node',
            namespace=config['namespace'],  # Unique namespace for each camera
            name=config['namespace'],  # Node name (optional)
            parameters=[{
            'video_device': config['device'],
            'image_size': [640, 360],
            'pixel_format': "YUYV",
            'framerate': 30,
        }],
            remappings=[
                ('/image_raw', f'/{config["namespace"]}/image_raw'),  # Remap output topic to the unique namespace
            ],
            output='screen'
        )
        nodes.append(node)

    # Return the launch description with both camera nodes
    return LaunchDescription(nodes)
