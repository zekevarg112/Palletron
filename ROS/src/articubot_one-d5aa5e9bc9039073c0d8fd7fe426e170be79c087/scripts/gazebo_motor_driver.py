#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import Float64, Float64MultiArray

class GazeboMotorDriver(Node):
    def __init__(self):
        super().__init__('gazebo_motor_driver')

        # Robot parameters
        self.wheelbase = 1.17       # Distance from steering shaft to fork wheels (m)
        self.wheel_separation = 0.22  # Distance between left and right steering wheels (m)
        self.wheel_radius = 0.1    # Radius of the steering wheels (m)

        # Publishers to controller command topics
        self.left_wheel_pub = self.create_publisher(
            Float64MultiArray, '/steering_wheel_left_controller/commands', 10)
        self.right_wheel_pub = self.create_publisher(
            Float64MultiArray, '/steering_wheel_right_controller/commands', 10)

        # Subscribers to wheel velocity topics
        self.left_wheel_sub = self.create_subscription(
            Float64, '/left_wheel_speed', self.left_wheel_callback, 10)
        self.right_wheel_sub = self.create_subscription(
            Float64, '/right_wheel_speed', self.right_wheel_callback, 10)

        self.get_logger().info('GazeboMotorDriver has started.')

    def left_wheel_callback(self, msg):
        # Create a Float64MultiArray message for the left wheel command
        left_wheel_cmd = Float64MultiArray()
        left_wheel_cmd.data = [(msg.data/self.wheel_radius)]  # The controller expects an array
        self.left_wheel_pub.publish(left_wheel_cmd)
        self.get_logger().debug(f'Left wheel command published: {msg.data}')

    def right_wheel_callback(self, msg):
        # Create a Float64MultiArray message for the right wheel command
        right_wheel_cmd = Float64MultiArray()
        right_wheel_cmd.data = [(msg.data/self.wheel_radius)]  # The controller expects an array
        self.right_wheel_pub.publish(right_wheel_cmd)
        self.get_logger().debug(f'Right wheel command published: {msg.data}')

def main(args=None):
    rclpy.init(args=args)
    node = GazeboMotorDriver()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
