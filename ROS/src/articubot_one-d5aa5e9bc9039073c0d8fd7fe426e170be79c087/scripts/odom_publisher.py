#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from nav_msgs.msg import Odometry
from geometry_msgs.msg import Quaternion, TransformStamped
from sensor_msgs.msg import JointState
import math
import tf2_ros

class PalletJackOdometryNode(Node):
    def __init__(self):
        super().__init__('pallet_jack_odometry_node')

        # Robot parameters
        self.wheelbase = 1.17       # Distance from steering shaft to fork wheels (m)
        self.wheel_separation = 0.22  # Distance between left and right steering wheels (m)
        self.wheel_radius = 0.1    # Radius of the steering wheels (m)

        # Robot state
        self.x = 0.0               # Position X
        self.y = 0.0               # Position Y
        self.theta = 0.0           # Orientation (yaw)

        self.last_time = self.get_clock().now()

        # Steering angle
        self.steering_angle = 0.0  # In radians

        # Wheel velocities
        self.left_wheel_velocity = 0.0  # m/s
        self.right_wheel_velocity = 0.0  # m/s

        # Create the odometry publisher
        self.odom_pub = self.create_publisher(Odometry, '/odom', 10)

        # Create a transform broadcaster for broadcasting odom -> base_link
        self.tf_broadcaster = tf2_ros.TransformBroadcaster(self)

        # Subscribe to the wheel joint states
        self.joint_state_sub = self.create_subscription(
            JointState, '/joint_states', self.joint_state_callback, 10)

        self.get_logger().info('PalletJackOdometryNode has started.')
        # Timer to run the control loop at a fixed frequency (e.g., 40 Hz)
        self.control_timer = self.create_timer(0.01, self.update_odometry)  # 0.025s = 40 Hz

    def joint_state_callback(self, msg):
        # Map joint names to indices
        joint_indices = {name: idx for idx, name in enumerate(msg.name)}

        # Extract steering angle
        steering_index = joint_indices.get('steering_shaft_joint')
        if steering_index is not None:
            self.steering_angle = msg.position[steering_index]

        # Extract wheel velocities
        left_wheel_index = joint_indices.get('steering_wheel_left_joint')
        right_wheel_index = joint_indices.get('steering_wheel_right_joint')

        if left_wheel_index is not None and right_wheel_index is not None:
            # Wheel angular velocities in rad/s
            left_wheel_angular_velocity = msg.velocity[left_wheel_index]
            right_wheel_angular_velocity = msg.velocity[right_wheel_index]

            # Convert to linear velocities (v = omega * r)
            self.left_wheel_velocity = left_wheel_angular_velocity * self.wheel_radius
            self.right_wheel_velocity = right_wheel_angular_velocity * self.wheel_radius

            # Update odometry
            #self.update_odometry()

    def update_odometry(self):
        current_time = self.get_clock().now()


        dt = (current_time - self.last_time).nanoseconds / 1e9  # Time step in seconds

        # Compute linear velocity of the front wheels (v_front)
        v_front = (self.left_wheel_velocity + self.right_wheel_velocity) / 2.0  # m/s
        #if (self.left_wheel_velocity > 0 and self.right_wheel_velocity < 0) or (self.left_wheel_velocity < 0 and self.right_wheel_velocity > 0):
        #    v_front = 0
        # Compute angular velocity (omega) using front-wheel-drive bicycle model
        if abs(self.wheelbase) > 1e-6:
            omega = (v_front / self.wheelbase) * math.sin(self.steering_angle)
            #omega = (v_front / self.wheelbase) * math.tan(self.steering_angle)

        else:
            omega = 0.0

        # Update robot pose
        delta_theta = omega * dt
        self.theta += delta_theta

        # Update x, y position considering front-wheel drive
        delta_x = v_front * math.cos(self.theta) * math.cos(self.steering_angle) * dt
        delta_y = v_front * math.sin(self.theta) * math.cos(self.steering_angle) * dt
        #delta_x = v_front * math.cos(self.theta) * dt
        #delta_y = v_front * math.sin(self.theta) * dt


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
        odom.twist.twist.linear.x = linear_velocity * math.cos(self.steering_angle)
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
