#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import Float64MultiArray

class LinearActuatorControllerNode(Node):
    def __init__(self):
        super().__init__('linear_actuator_controller_node')

        # Desired effort to apply
        self.desired_effort = 0.0

        # Subscription to joystick input
        self.joy_sub = self.create_subscription(
            Joy, '/joy', self.joy_callback, 10)

        # Create publishers to send effort commands to all actuators
        self.linear_actuator_pub = self.create_publisher(
            Float64MultiArray, '/linear_actuator_controller/commands', 10)

#        self.fork_right_actuator_pub = self.create_publisher(
#            Float64MultiArray, '/fork_right_actuator_controller/commands', 10)

#        self.fork_left_actuator_pub = self.create_publisher(
#            Float64MultiArray, '/fork_left_actuator_controller/commands', 10)

        # Create a timer to publish commands at regular intervals (e.g., 10 Hz)
        self.timer = self.create_timer(0.1, self.publish_command)

        self.get_logger().info('Linear Actuator Controller Node has started.')

    def joy_callback(self, msg):
        # Check if A button (index 0) is pressed to apply 5000 N effort
        if msg.buttons[0] == 1:
            self.desired_effort = 5000.0  # Apply 5000 N effort
            self.get_logger().info(f'Applying effort: {self.desired_effort} N')

        # Check if B button (index 1) is pressed to stop actuator (0 N effort)
        elif msg.buttons[1] == 1:
            self.desired_effort = 0.0  # Stop effort
            self.get_logger().info('Stopping actuator (0 N effort)')

    def publish_command(self):
        # Create the Float64MultiArray message
        command_msg = Float64MultiArray()
        command_msg.data = [self.desired_effort]
        #neg_command_msg = Float64MultiArray()
        #neg_command_msg.data = [-self.desired_effort]

        # Publish the command to all actuators
        self.linear_actuator_pub.publish(command_msg)
        #self.fork_right_actuator_pub.publish(neg_command_msg)
        #self.fork_left_actuator_pub.publish(neg_command_msg)

def main(args=None):
    rclpy.init(args=args)
    node = LinearActuatorControllerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
