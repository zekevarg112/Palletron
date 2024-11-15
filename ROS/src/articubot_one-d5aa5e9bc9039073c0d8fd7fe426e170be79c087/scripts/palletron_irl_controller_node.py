#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from std_msgs.msg import Float32
from motor_interfaces.srv import SetMotorRpm
import math

class PalletJackControllerNode(Node):
    def __init__(self):
        super().__init__('pallet_jack_controller_node')

        # Robot parameters
        self.wheelbase = 1.17            # Distance from steering shaft to fork wheels (m)
        self.wheel_separation = 0.18     # Distance between left and right steering wheels (m)
        self.max_speed = 140             # Max RPM (RPM)
        self.max_steering_angle = math.radians(85)  # Max steering angle (adjust as needed)
        self.wheel_radius = 0.1075       # Radius of the steering wheels (m)
        self.last_right_wheel_rpm = 0
        self.last_left_wheel_rpm = 0

        # Steering control parameters
        self.steering_kp = 0.5           # Proportional gain (tune as needed)
        
        # Create a client for the set_motor_rpm service
        self.client = self.create_client(SetMotorRpm, 'set_motor_rpm')
        while not self.client.wait_for_service(timeout_sec=10.0):
            self.get_logger().info('Waiting for set_motor_rpm service...')
        self.get_logger().info('Started.')

        # Steering angle and velocity
        self.steering_angle = 0.0
        self.cmd_vel = Twist()  # Initialize with zero velocities

        # Subscribe to the angle sensor
        self.encoder_angle_sub = self.create_subscription(
            Float32,
            '/encoder_angle',
            self.encoder_angle_callback,
            10
        )
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

        # Subscribe to cmd_vel
        self.cmd_vel_sub = self.create_subscription(
            Twist, '/cmd_vel', self.cmd_vel_callback, 10
        )

        # Timer to run the control loop at a fixed frequency (e.g., 40 Hz)
        self.control_timer = self.create_timer(0.025, self.control_loop)  # 0.025s = 40 Hz

        self.get_logger().info('PalletJackControllerNode has started.')

    
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
        # Update steering angle from sensor feedback
        self.steering_angle = msg.data

    def cmd_vel_callback(self, msg):
        # Update the latest cmd_vel message
        self.cmd_vel = msg

    def rpm_to_duty_cycle(self, rpm):
        result = 0
        # Coefficients from the curve fitting
        
        a = 0.00101248
        b = 0.56673238

        if rpm == 0:
            result = 0
        elif rpm < 0:
            result = -(a * abs(rpm)**2 + b * abs(rpm))/100
        else:
            result = (a * rpm**2 + b * rpm)/100
        return result

    def control_loop(self):
        # Extract linear and angular velocities from the latest cmd_vel
        v = self.cmd_vel.linear.x      # Linear velocity (m/s)
        omega = self.cmd_vel.angular.z # Angular velocity (rad/s)

        # Compute desired steering angle based on cmd_vel
        delta_desired = math.atan2(self.wheelbase * omega, abs(v)) if abs(v) > 1e-4 else 0.0

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
        rpm_left = ((v - (v_steering / 2.0)) * 60) / (2 * math.pi * self.wheel_radius)
        rpm_right = ((v + (v_steering / 2.0)) * 60) / (2 * math.pi * self.wheel_radius)



        # Limit wheel speeds to maximum allowable speed
        rpm_left = max(min(rpm_left, self.max_speed), -self.max_speed)
        rpm_right = max(min(rpm_right, self.max_speed), -self.max_speed)

        #rpm_left = 60
        #rpm_right = 60

        self.get_logger().info(f"Right Motor RPM: {rpm_right:.2f}")
        self.get_logger().info(f"Left Motor RPM: {rpm_left:.2f}")

        duty_cycle_left = self.rpm_to_duty_cycle(rpm_left)
        duty_cycle_right = self.rpm_to_duty_cycle(rpm_right)

        #self.get_logger().info(f"Output from function RIGHT: {duty_cycle_right:.2f}")
        #self.get_logger().info(f"Output from function LEFT: {duty_cycle_left:.2f}")

        #duty_cycle_left = 0.1
        #duty_cycle_right = 0.1

        # Send motor command
        #self.send_motor_command(rpm_right, rpm_left)
        self.send_motor_command(duty_cycle_right, duty_cycle_left)

    


    def send_motor_command(self, rpm_right, rpm_left):
        # Create request for the set_motor_rpm service
        request = SetMotorRpm.Request()
        request.command.right = float(rpm_right)
        request.command.left = float(rpm_left)
        #self.get_logger().info(f"Right Motor RPM: {rpm_left:.2f}")
        # Send request asynchronously
        future = self.client.call_async(request)
        future.add_done_callback(self.service_callback)

    def service_callback(self, future):
        try:
            response = future.result()
            if not response.success:
                self.get_logger().error('Failed to send Motor RPM command')
        except Exception as e:
            self.get_logger().error(f'Service call failed: {e}')


def main(args=None):
    rclpy.init(args=args)
    node = PalletJackControllerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
