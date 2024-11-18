#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import Bool
from motor_interfaces.srv import SetMotorRpm
import socket
import time


class FailsafeNode(Node):
    def __init__(self):
        super().__init__('failsafe_node')

        # Subscriber for ros_controller_check
        self.ros_controller_check_sub = self.create_subscription(Bool, 'ros_controller_check', self.ros_controller_check_callback, 10)

        # Timers for checking flags
        self.ros_check_timer = self.create_timer(0.2, self.check_ros_controller_flag)
        self.internet_check_timer = self.create_timer(0.2, self.check_internet_connection)

        # Flags
        self.last_ros_check_time = time.time()
        self.ros_controller_flag = False
        self.vesc_check_flag = False
        self.internet_connected = False

        # Client for set_motor_rpm service
        self.client = self.create_client(SetMotorRpm, 'set_motor_rpm')
        #while not self.client.wait_for_service(timeout_sec=1.0):
        #    self.get_logger().info('Waiting for set_motor_rpm service...')
        self.get_logger().info('FailsafeNode has started.')

    def ros_controller_check_callback(self, msg):
        if msg.data:
            self.last_ros_check_time = time.time()
            self.ros_controller_flag = False  # Reset the flag when a valid message is received
            self.vesc_check_flag = False  # Reset vesc_check_flag since we received True
            self.get_logger().debug("Received ros_controller_check: True")
        else:
            self.vesc_check_flag = True  # Set vesc_check_flag if the message is False
            self.get_logger().warn("Received ros_controller_check: False. Setting vesc_check_flag.")

    def check_ros_controller_flag(self):
        # Check if more than 0.2 seconds have passed since the last ros_controller_check message
        if time.time() - self.last_ros_check_time > 0.2:
            self.ros_controller_flag = True
            self.get_logger().warn('ROS Controller check flag is set. No message received in 0.2 seconds.')

        # Trigger failsafe if any flag is set
        if self.ros_controller_flag or self.vesc_check_flag or not self.internet_connected:
            self.trigger_failsafe()

    def check_internet_connection(self):
        # Try to connect to a well-known website (e.g., Google) to test internet connection
        try:
            socket.create_connection(("8.8.8.8", 53), timeout=1)
            self.internet_connected = True
            self.get_logger().debug("Internet is connected.")
        except OSError:
            self.internet_connected = False
            self.get_logger().warn("No internet connection. Setting internet_connected flag to False.")

    def trigger_failsafe(self):
        # Log failsafe state
        self.get_logger().info(
            "Failsafe triggered. Flags: "
            f"ros_controller_flag={self.ros_controller_flag}, "
            f"vesc_check_flag={self.vesc_check_flag}, "
            f"internet_connected={self.internet_connected}."
        )

        # Send 0, 0 to the motors
        request = SetMotorRpm.Request()
        request.command.right = 0.0
        request.command.left = 0.0
        future = self.client.call_async(request)
        future.add_done_callback(self.failsafe_callback)

    def failsafe_callback(self, future):
        try:
            response = future.result()
            if not response.success:
                self.get_logger().error('Failsafe command failed to send to motors.')
            else:
                self.get_logger().info('Failsafe command successfully sent to motors.')
        except Exception as e:
            self.get_logger().error(f'Service call failed: {e}')


def main(args=None):
    rclpy.init(args=args)
    node = FailsafeNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()


if __name__ == '__main__':
    main()
