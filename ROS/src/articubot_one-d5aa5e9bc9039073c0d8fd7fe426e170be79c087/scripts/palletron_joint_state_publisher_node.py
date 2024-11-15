#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import JointState
from std_msgs.msg import Float32
import math

class PalletronJointStatePublisherNode(Node):
    def __init__(self):
        super().__init__('palletron_joint_state_publisher_node')

        # Subscribe to the motor velocities and encoder angle topics
        self.right_motor_sub = self.create_subscription(
            Float32,
            '/right_motor_velocity',
            self.right_motor_callback,
            10
        )
        self.left_motor_sub = self.create_subscription(
            Float32,
            '/left_motor_velocity',
            self.left_motor_callback,
            10
        )
        self.encoder_angle_sub = self.create_subscription(
            Float32,
            '/encoder_angle',
            self.encoder_angle_callback,
            10
        )

        # Publisher for joint states
        self.joint_state_pub = self.create_publisher(JointState, 'joint_states', 10)

        # Initialize variables
        self.steering_angle = 0.0       # From encoder_angle topic (radians)
        self.right_wheel_rpm = 0.0      # From right_motor_velocity topic (RPM)
        self.left_wheel_rpm = 0.0       # From left_motor_velocity topic (RPM)

        self.last_right_wheel_rpm = 0
        self.last_left_wheel_rpm = 0

        self.right_wheel_position = 0.0  # Accumulated position (radians)
        self.left_wheel_position = 0.0   # Accumulated position (radians)

        self.last_time = self.get_clock().now()

        # Timer to publish joint states at a fixed rate (e.g., 40 Hz)
        self.timer = self.create_timer(0.025, self.publish_joint_states)  # 40 Hz

    def right_motor_callback(self, msg):
        if abs(self.last_right_wheel_rpm-msg.data)>150:
            self.right_wheel_rpm = self.last_right_wheel_rpm
        else:
            self.right_wheel_rpm = msg.data
        self.last_right_wheel_rpm = self.right_wheel_rpm
        #self.get_logger().info(f"Right Motor RPM: {self.right_wheel_rpm:.2f}")

    def left_motor_callback(self, msg):
        #self.last_left_wheel_rpm = 0
        if abs(self.last_left_wheel_rpm-msg.data)>150:
            self.left_wheel_rpm = self.last_left_wheel_rpm
        else:
            self.left_wheel_rpm = msg.data
        self.last_left_wheel_rpm = self.left_wheel_rpm

    def encoder_angle_callback(self, msg):
        self.steering_angle = msg.data

    def publish_joint_states(self):
        current_time = self.get_clock().now()
        dt = (current_time - self.last_time).nanoseconds / 1e9  # Convert nanoseconds to seconds

        # Convert RPM to radians per second
        left_wheel_rad_per_sec = (self.left_wheel_rpm * 2 * math.pi) / 60.0
        right_wheel_rad_per_sec = (self.right_wheel_rpm * 2 * math.pi) / 60.0

        # Update wheel positions by integrating angular velocity over time
        self.left_wheel_position += left_wheel_rad_per_sec * dt
        self.right_wheel_position += right_wheel_rad_per_sec * dt

        # Prepare and publish the JointState message
        joint_state_msg = JointState()
        joint_state_msg.header.stamp = current_time.to_msg()
        joint_state_msg.name = ['steering_shaft_joint', 'steering_wheel_right_joint', 'steering_wheel_left_joint']
        joint_state_msg.position = [self.steering_angle, self.right_wheel_position, self.left_wheel_position]

        self.joint_state_pub.publish(joint_state_msg)

        self.last_time = current_time  # Update the last time

def main(args=None):
    rclpy.init(args=args)
    node = PalletronJointStatePublisherNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
