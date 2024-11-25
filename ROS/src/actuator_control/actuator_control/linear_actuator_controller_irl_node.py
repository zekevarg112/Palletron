#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import Joy
from std_msgs.msg import Float32

class LinearActuatorControllerNode(Node):
    def __init__(self):
        super().__init__('linear_actuator_controller_node')

        # Subscription to joystick input
        self.joy_sub = self.create_subscription(
            Joy, '/joy', self.joy_callback, 10
        )

        # Publisher to send commands to the actuator control topic
        self.actuator_command_pub = self.create_publisher(
            Float32, '/actuator_command', 10
        )

        # Variables to store the previous state of A and B buttons
        self.a_pressed = False
        self.b_pressed = False

        self.get_logger().info('Linear Actuator Controller Node has started.')

    def joy_callback(self, msg):
        # Current state of A and B buttons
        current_a_state = msg.buttons[0] == 1  # True if A button is pressed
        current_b_state = msg.buttons[1] == 1  # True if B button is pressed

        # Check if A button (index 0) is pressed and wasn't already pressed
        if current_a_state and not self.a_pressed:
            command = 1.0  # Command for actuator
            self.publish_command(command)
            self.get_logger().info(f'Sending command to actuator: {command}')
            self.a_pressed = True  # Mark A as pressed

        # Check if B button (index 1) is pressed and wasn't already pressed
        elif current_b_state and not self.b_pressed:
            command = 2.0  # Command for actuator
            self.publish_command(command)
            self.get_logger().info(f'Sending command to actuator: {command}')
            self.b_pressed = True  # Mark B as pressed

        # Reset the press state when the button is released
        if not current_a_state:
            self.a_pressed = False
        if not current_b_state:
            self.b_pressed = False

    def publish_command(self, command):
        # Create the Float32 message
        command_msg = Float32()
        command_msg.data = command

        # Publish the command to the actuator control topic
        self.actuator_command_pub.publish(command_msg)

def main(args=None):
    rclpy.init(args=args)
    node = LinearActuatorControllerNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
