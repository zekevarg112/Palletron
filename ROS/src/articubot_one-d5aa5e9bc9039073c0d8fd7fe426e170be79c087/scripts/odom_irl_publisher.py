#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Quaternion, TransformStamped
from sensor_msgs.msg import JointState
import math
import tf2_ros
from std_msgs.msg import Float32

class PalletJackOdometryNode(Node):
    def __init__(self):
        super().__init__('pallet_jack_odometry_node')

        # Robot parameters
        self.wheelbase = 1.17       # Distance from steering shaft to fork wheels (m)
        self.wheel_separation = 0.18  # Distance between left and right steering wheels (m)
        self.wheel_radius = 0.1075    # Radius of the steering wheels (m)
        self.last_right_wheel_rpm = 0
        self.last_left_wheel_rpm = 0

        # Robot state
        self.x = 0.0               # Position X
        self.y = 0.0               # Position Y
        self.theta = 0.0           # Orientation (yaw)

        self.last_time = self.get_clock().now()

        # Steering angle
        self.encoder_angle = 0.0  # In radians

        # Wheel velocities
        self.left_wheel_velocity = 0.0  # m/s
        self.right_wheel_velocity = 0.0  # m/s

        # Create the odometry publisher
        self.odom_pub = self.create_publisher(Odometry, '/odom', 10)

        # Create a transform broadcaster for broadcasting odom -> base_link
        self.tf_broadcaster = tf2_ros.TransformBroadcaster(self)

        # Subscribers for motor velocities and encoder angle
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
        # Timer to run the control loop at a fixed frequency (e.g., 40 Hz)
        self.control_timer = self.create_timer(0.025, self.update_odometry)  # 0.025s = 40 Hz

        self.get_logger().info('PalletJackOdometryNode has started.')

    def right_motor_callback(self, msg):
        if abs(self.last_right_wheel_rpm-msg.data)>150:
            self.right_wheel_rpm = self.last_right_wheel_rpm
        else:
            self.right_wheel_rpm = msg.data
        self.last_right_wheel_rpm = self.right_wheel_rpm
        self.right_wheel_velocity = (self.right_wheel_rpm / 60) * (2 * math.pi * self.wheel_radius)
        #self.get_logger().info(f"Right Motor RPM: {self.right_wheel_rpm:.2f}")

    def left_motor_callback(self, msg):
        #self.last_left_wheel_rpm = 0
        if abs(self.last_left_wheel_rpm-msg.data)>150:
            self.left_wheel_rpm = self.last_left_wheel_rpm
        else:
            self.left_wheel_rpm = msg.data
        self.last_left_wheel_rpm = self.left_wheel_rpm
        self.left_wheel_velocity = (self.left_wheel_rpm / 60) * (2 * math.pi * self.wheel_radius)
        

    def encoder_angle_callback(self, msg):
        # Process encoder angle
        self.encoder_angle = msg.data
        self.get_logger().info(f"Encoder Angle: {self.encoder_angle}")

        # Update odometry
        #self.update_odometry()


    def update_odometry(self):
        current_time = self.get_clock().now()
        #self.get_logger().info(f"Right Motor omega: {self.right_wheel_velocity:.2f}")
        #self.get_logger().info(f"Left Motor omega: {self.left_wheel_velocity:.2f}")
        dt = (current_time - self.last_time).nanoseconds / 1e9  # Time step in seconds

        # Compute linear velocity of the front wheels (v_front)
        v_front = (self.left_wheel_velocity + self.right_wheel_velocity) / 2.0  # m/s

        # Compute angular velocity (omega) using front-wheel-drive bicycle model
        if abs(self.wheelbase) > 1e-6:
            omega = (v_front / self.wheelbase) * math.sin(self.encoder_angle)

        else:
            omega = 0.0

        # Update robot pose
        delta_theta = omega * dt
        self.theta += delta_theta

        # Update x, y position considering front-wheel drive
        delta_x = v_front * math.cos(self.theta) * math.cos(self.encoder_angle) * dt
        delta_y = v_front * math.sin(self.theta) * math.cos(self.encoder_angle) * dt

        self.x += delta_x
        self.y += delta_y

        # Publish odometry message
        self.publish_odometry(v_front, omega, current_time)

        # Publish TF transform
        self.publish_transform(current_time)

        # Update last_time
        self.last_time = current_time


    def publish_odometry(self, linear_velocity, angular_velocity, current_time):
        # Create and populate the odometry message
        odom = Odometry()
        odom.header.stamp = current_time.to_msg()
        odom.header.frame_id = 'odom'
        odom.child_frame_id = 'base_link'

        # Set the position and orientation in odom message
        odom.pose.pose.position.x = self.x
        odom.pose.pose.position.y = self.y
        odom.pose.pose.position.z = 0.0
        quaternion = self.yaw_to_quaternion(self.theta)
        odom.pose.pose.orientation = quaternion

        # Set the velocity in odom message
        #odom.twist.twist.linear.x = linear_velocity
        odom.twist.twist.linear.x = linear_velocity * math.cos(self.encoder_angle)  # Projected forward velocity
        odom.twist.twist.linear.y = 0.0
        odom.twist.twist.angular.z = angular_velocity

        # Publish the odometry message
        self.odom_pub.publish(odom)

    def publish_transform(self, current_time):
        # Create the transform stamped message
        transform = TransformStamped()
        transform.header.stamp = current_time.to_msg()
        transform.header.frame_id = 'odom'
        transform.child_frame_id = 'base_link'
        transform.transform.translation.x = self.x
        transform.transform.translation.y = self.y
        transform.transform.translation.z = 0.0
        quaternion = self.yaw_to_quaternion(self.theta)
        transform.transform.rotation = quaternion

        # Broadcast the transform
        self.tf_broadcaster.sendTransform(transform)

    def yaw_to_quaternion(self, yaw):
        # Convert yaw angle to quaternion
        half_yaw = yaw * 0.5
        quat = Quaternion()
        quat.x = 0.0
        quat.y = 0.0
        quat.z = math.sin(half_yaw)
        quat.w = math.cos(half_yaw)
        return quat

def main(args=None):
    rclpy.init(args=args)
    node = PalletJackOdometryNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
