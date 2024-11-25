#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist 
from std_msgs.msg import Float64
from sensor_msgs.msg import JointState
import math

class PalletJackControllerNode(Node):
    def __init__(self):
        super().__init__('pallet_jack_controller_node')

        # Robot parameters
        self.wheelbase = 1.17            # Distance from steering shaft to fork wheels (m)
        self.wheel_separation = 0.22     # Distance between left and right steering wheels (m)
        self.max_speed = 1.5             # Max linear speed (m/s)
        self.max_steering_angle = math.radians(89)  # Max steering angle (adjust as needed)

        # Steering control parameters
        self.steering_kp = 0.8  # Proportional gain (tune as needed)

        # Publishers for wheel speeds
        self.left_wheel_speed_pub = self.create_publisher(Float64, '/left_wheel_speed', 10)
        self.right_wheel_speed_pub = self.create_publisher(Float64, '/right_wheel_speed', 10)

        self.v = 0
        self.omega = 0

        # Steering angle
        self.steering_angle = 0.0  # In radians
        # Subscribe to the wheel joint states
        self.joint_state_sub = self.create_subscription(
            JointState, '/joint_states', self.joint_state_callback, 10)
        
        # Subscribe to cmd_vel
        self.cmd_vel_sub = self.create_subscription(
            Twist, '/cmd_vel', self.cmd_vel_callback, 100)

        self.get_logger().info('PalletJackControllerNode has started.')
        # Timer to run the control loop at a fixed frequency (e.g., 40 Hz)
        self.control_timer = self.create_timer(0.01, self.control_loop)  # 0.025s = 40 Hz

    def cmd_vel_callback(self, msg):
        # Extract linear and angular velocities from cmd_vel
        self.v = msg.linear.x      # Linear velocity (m/s)
        self.omega = msg.angular.z # Angular velocity (rad/s)
        #self.control_loop()

    
    def control_loop(self):# Compute desired steering angle
        delta_desired = math.atan2(self.wheelbase * self.omega, abs(self.v)) if abs(self.v) > 1e-4 else 0.0

        # Limit desired steering angle
        delta_desired = max(min(delta_desired, self.max_steering_angle), -self.max_steering_angle)

        # Steering angle control (simple proportional controller)
        steering_error = delta_desired - self.steering_angle
        steering_control = self.steering_kp * steering_error

        # Compute wheel speed difference for steering
        v_steering = steering_control

        # Limit v_steering to avoid excessive wheel speed differences
        max_steering_speed = self.max_speed
        v_steering = max(min(v_steering, max_steering_speed), -max_steering_speed)

        # Compute individual wheel speeds
        v_left = self.v - (v_steering / 2.0)
        v_right = self.v + (v_steering / 2.0)

        # Limit wheel speeds to maximum allowable speed
        v_left = max(min(v_left, self.max_speed), -self.max_speed)
        v_right = max(min(v_right, self.max_speed), -self.max_speed)
#
#        # Compute desired wheel velocities
#        v_left, v_right = self.compute_wheel_velocities(v, omega)
#
        # Publish left wheel speed
        left_wheel_msg = Float64()
        left_wheel_msg.data = v_left
        self.left_wheel_speed_pub.publish(left_wheel_msg)

        # Publish right wheel speed
        right_wheel_msg = Float64()
        right_wheel_msg.data = v_right
        self.right_wheel_speed_pub.publish(right_wheel_msg)


    def joint_state_callback(self, msg):
        # Map joint names to indices
        joint_indices = {name: idx for idx, name in enumerate(msg.name)}

        # Extract steering angle
        steering_index = joint_indices.get('steering_shaft_joint')
        if steering_index is not None:
            self.steering_angle = msg.position[steering_index]


    def compute_wheel_velocities(self, v, omega):
        # Compute the steering angle delta based on robot geometry
        
        if abs(omega) > 1e-6:
            R = v / omega  # Turning radius
        else:
            R = float('inf')  # Straight line motion

        if R != float('inf'):
            # Distance from the pivot point (ICC) to each wheel
            R_left = R - (self.wheel_separation / 2.0)
            R_right = R + (self.wheel_separation / 2.0)

            # Compute wheel velocities
            v_left = omega * R_left
            v_right = omega * R_right
        else:
            # Straight line motion
            v_left = v
            v_right = v

        if abs(self.steering_angle) > 1.483 and v < 0.2 and abs(omega) > 0.2:
            v_left = abs(omega)
            v_right = abs(omega)

        # Limit wheel speeds to maximum allowable speed
        max_wheel_speed = max(abs(v_left), abs(v_right))
        if max_wheel_speed > self.max_speed:
            scaling_factor = self.max_speed / max_wheel_speed
            v_left *= scaling_factor
            v_right *= scaling_factor

        return v_left, v_right

def main(args=None):
    rclpy.init(args=args)
    node = PalletJackControllerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
