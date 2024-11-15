#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32  # Keep Float32 for incoming messages
import serial
import serial.tools.list_ports

class ArduinoActuatorControl(Node):
    def __init__(self):
        super().__init__('arduino_actuator_control')
        self.subscription = self.create_subscription(
            Float32,  # Subscribe to Float32 messages
            '/actuator_command',
            self.listener_callback,
            10
        )
        self.serial_port = self.find_arduino_serial_port()
        if self.serial_port is not None:
            try:
                self.arduino_serial = serial.Serial(self.serial_port, 9600)
                self.get_logger().info(f'Connected to Arduino Uno on port {self.serial_port}')
            except serial.SerialException as e:
                self.get_logger().error(f'Failed to connect to Arduino Uno on port {self.serial_port}: {e}')
                exit(1)
        else:
            self.get_logger().error('Arduino Uno not found. Please check the connection.')
            exit(1)

    def find_arduino_serial_port(self):
        ports = serial.tools.list_ports.comports()
        for port in ports:
            # Uncomment the line below to see all available ports in the log
            # self.get_logger().info(f'Checking port {port.device}, desc: {port.description}, hwid: {port.hwid}')
            # Check for Arduino Uno by VID:PID (2341:0043 for official Uno, 1A86:7523 for CH340-based Uno clones)
            if 'USB VID:PID=2341:0043' in port.hwid or 'USB VID:PID=1A86:7523' in port.hwid:
                return port.device
        return None

    def listener_callback(self, msg):
        command = int(msg.data)  # Cast the float to an integer
        if command in [1, 2]:  # Check if the integer command is 1 or 2
            self.get_logger().info(f'Sending command to Arduino: {command}')
            self.arduino_serial.write(f'{command}\n'.encode())  # Send command as an integer string
        else:
            self.get_logger().warn(f'Invalid command received: {msg.data}')

def main(args=None):
    rclpy.init(args=args)
    arduino_actuator_control = ArduinoActuatorControl()
    rclpy.spin(arduino_actuator_control)
    arduino_actuator_control.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
