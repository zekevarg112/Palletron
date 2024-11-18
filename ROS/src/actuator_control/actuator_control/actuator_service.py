import rclpy
from rclpy.node import Node
import serial
import struct
import threading
import glob
import time
from std_msgs.msg import Float32

# Service
from actuator_service.srv import actuatorCommand

class ActuatorControllerNode(Node):
    def __init__(self):
        super().__init__('motor_controller_node')

         # Declare ROS parameters for baud rate and handshake response byte
        self.declare_parameter('baud_rate', 115200)

        # Get the parameter values
        self.baud_rate = self.get_parameter('baud_rate').get_parameter_value().integer_value
        

        
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


        # Create service to receive RPM commands
        self.srv = self.create_service(actuatorCommand, 'actuator_service', self.service_callback)
        self.get_logger().info('Started service')

        
    def find_arduino_serial_port(self):
        ports = serial.tools.list_ports.comports()
        for port in ports:
            # Uncomment the line below to see all available ports in the log
            # self.get_logger().info(f'Checking port {port.device}, desc: {port.description}, hwid: {port.hwid}')
            # Check for Arduino Uno by VID:PID (2341:0043 for official Uno, 1A86:7523 for CH340-based Uno clones)
            if 'USB VID:PID=2341:0043' in port.hwid or 'USB VID:PID=1A86:7523' in port.hwid:
                return port.device
        return None
        

    def service_callback(self, request, response):
        command = int(request.command)  # Cast the float to an integer
        if command in [1, 2]:  # Check if the integer command is 1 or 2
            self.get_logger().info(f'Sending command to Arduino: {command}')
            self.arduino_serial.write(f'{command}\n'.encode())  # Send command as an integer string
        else:
            self.get_logger().warn(f'Invalid command received: {request.command}')
        
        # Send RPM commands to motors
        
        response.success = True
        return response



        
def main(args=None):
    rclpy.init(args=args)
    
    linak_controller_node = ActuatorControllerNode()
    
    try:
        rclpy.spin(linak_controller_node)
    except KeyboardInterrupt:
        pass
    finally:
        # Cleanup
        linak_controller_node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
