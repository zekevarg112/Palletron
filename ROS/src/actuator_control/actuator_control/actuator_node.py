#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32, String  # Float32 for commands, String for status updates
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
        self.status_publisher = self.create_publisher(
            String,  # Publish actuator status as a String
            '/actuator_status',
            10
        )

        self.serial_port = self.find_arduino_serial_port()
        if self.serial_port is not None:
            try:
                self.arduino_serial = serial.Serial(self.serial_port, 115200, timeout=0.1)
                self.get_logger().info(f'Connected to Arduino Uno on port {self.serial_port}')
            except serial.SerialException as e:
                self.get_logger().error(f'Failed to connect to Arduino Uno on port {self.serial_port}: {e}')
                exit(1)
        else:
            self.get_logger().error('Arduino Uno not found. Please check the connection.')
            exit(1)

        # Timer to periodically check Arduino responses
        self.timer = self.create_timer(0.1, self.read_arduino_response)

    def find_arduino_serial_port(self):
        ports = serial.tools.list_ports.comports()
        for port in ports:
            # Uncomment to log available ports
            # self.get_logger().info(f'Checking port {port.device}, desc: {port.description}, hwid: {port.hwid}')
            if 'USB VID:PID=2341:0043' in port.hwid or 'USB VID:PID=1A86:7523' in port.hwid:
                return port.device
        return None

    def listener_callback(self, msg):
        command = int(msg.data)  # Cast the float to an integer
        if command in [0, 1, 2]:  # Valid commands: 0 (stop), 1 (extend), 2 (retract)
            self.get_logger().info(f'Sending command to Arduino: {command}')
            self.arduino_serial.write(f'{command}\n'.encode())  # Send command to Arduino
        else:
            self.get_logger().warn(f'Invalid command received: {msg.data}')

    def read_arduino_response(self):
        if self.arduino_serial.in_waiting > 0:  # Check if data is available
            response = self.arduino_serial.readline().decode().strip()
            if response.isdigit():
                response_code = int(response)
                status_message = None

                # Map Arduino response codes to status messages
                if response_code == 0:
                    status_message = "Actuator stopped"
                elif response_code == 1:
                    status_message = "Actuator fully extended"
                elif response_code == 2:
                    status_message = "Actuator fully retracted"

                if status_message:
                    self.get_logger().info(f'Received from Arduino: {status_message}')
                    self.status_publisher.publish(String(data=status_message))
            else:
                self.get_logger().warn(f'Unexpected response from Arduino: {response}')

def main(args=None):
    rclpy.init(args=args)
    arduino_actuator_control = ArduinoActuatorControl()
    rclpy.spin(arduino_actuator_control)
    arduino_actuator_control.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
