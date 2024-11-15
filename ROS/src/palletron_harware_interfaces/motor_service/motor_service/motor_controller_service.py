import rclpy
from rclpy.node import Node
import serial
import struct
import threading
import glob
import time
from std_msgs.msg import Float32

# Service
from motor_interfaces.srv import SetMotorRpm

class MotorControllerNode(Node):
    def __init__(self):
        super().__init__('motor_controller_node')

         # Declare ROS parameters for baud rate and handshake response byte
        self.declare_parameter('baud_rate', 115200)
        self.declare_parameter('handshake_response_byte', 0xA6)
        self.declare_parameter('timeout', 2.0)  # Timeout in seconds
        self.declare_parameter('publish_rate', 10.0)  # Publish rate in Hz
        self.declare_parameter('right_motor_topic', 'right_motor_velocity')
        self.declare_parameter('left_motor_topic', 'left_motor_velocity')
        self.declare_parameter('encoder_angle_topic', 'encoder_angle')

        # Get the parameter values
        self.baud_rate = self.get_parameter('baud_rate').get_parameter_value().integer_value
        self.handshake_response_byte = self.get_parameter('handshake_response_byte').get_parameter_value().integer_value
        self.timeout = self.get_parameter('timeout').get_parameter_value().double_value
        self.publish_rate_value = self.get_parameter('publish_rate').get_parameter_value().double_value
        self.right_motor_topic = self.get_parameter('right_motor_topic').get_parameter_value().string_value
        self.left_motor_topic = self.get_parameter('left_motor_topic').get_parameter_value().string_value
        self.encoder_angle_topic = self.get_parameter('encoder_angle_topic').get_parameter_value().string_value


        # Initialize publishers for motor velocities and encoder angle
        self.right_motor_pub = self.create_publisher(Float32, self.right_motor_topic , 10)
        self.left_motor_pub = self.create_publisher(Float32, self.left_motor_topic, 10)
        self.encoder_angle_pub = self.create_publisher(Float32, self.encoder_angle_topic, 10)
        



        self.right_motor_rpm = 0.0
        self.left_motor_rpm = 0.0
        self.encoder_angle = 0.0
        

        
        # Auto-detect serial port using handshake
        self.serial_conn = self.auto_detect_serial_port()

        if self.serial_conn is None:
            self.get_logger().error("No valid serial port found after handshake. Proceeding in dummy mode.")
            self.dummy = True
        else:
            self.dummy = False
            # Start a separate thread to read from the serial port
            self.serial_thread = threading.Thread(target=self.read_serial_data, daemon=True)
            self.serial_thread.start()


        # Create service to receive RPM commands
        self.srv = self.create_service(SetMotorRpm, 'set_motor_rpm', self.handle_motor_command)
        self.get_logger().info('Started service')

        # Start a thread to publish the motor velocities and encoder angle
        self.publish_thread = threading.Thread(target=self.publish_data, daemon=True)
        self.publish_thread.start()
        self.get_logger().info('Started state publishers')

    def auto_detect_serial_port(self):
        # Define possible patterns for serial devices
        possible_ports = glob.glob('/dev/ttyACM*') + glob.glob('/dev/ttyUSB*')

        for port in possible_ports:
            try:
                self.get_logger().info(f"Trying {port} with baud rate {self.baud_rate} for handshake...")
                conn = serial.Serial(port, self.baud_rate, timeout=1)

                if conn.is_open:
                    if self.ping_device(conn):
                        self.get_logger().info(f"Handshake successful with device on {port}")
                        return conn  # Return the working serial connection
                    else:
                        self.get_logger().warn(f"No valid response from {port} during handshake.")
                conn.close()  # Close the connection if handshake fails
            except serial.SerialException as e:
                self.get_logger().warn(f"Failed to connect to {port}: {e}")

        return None  # No valid ports found

    def ping_device(self, conn):
        try:
            # Construct the 7-byte handshake packet (first byte is 0xA6, others are 0)
            handshake_packet = struct.pack('B', 0xA6) + b'\x00' * 6  # 0xA6 + six zeros

            conn.reset_input_buffer()  # Clear the input buffer before sending the handshake
            conn.write(handshake_packet)  # Send the full 7-byte packet
            start_time = time.time()  # Record the start time

            self.get_logger().debug(f"Sent handshake packet: {handshake_packet.hex()}")

            # Wait for the response with a timeout
            while (time.time() - start_time) < self.timeout:  # Keep reading until the timeout
                if conn.in_waiting > 0:  # Check if there's data to read
                    response = conn.read(1)  # Read one byte at a time
                    if response == bytes([self.handshake_response_byte]):
                        self.get_logger().debug(f"Handshake successful: Received {hex(self.handshake_response_byte)}")
                        return True
                time.sleep(0.001)  # Small delay to avoid excessive CPU usage

            self.get_logger().warn("Handshake failed: Timeout while waiting for response")
            return False
        except Exception as e:
            self.get_logger().error(f"Error during handshake: {e}")
            return False
        

    def handle_motor_command(self, request, response):
        rpm_right = request.command.right
        rpm_left = request.command.left
        
        # Send RPM commands to motors
        self.send_rpm_command(rpm_right, rpm_left)
        
        response.success = True
        return response

    def send_rpm_command(self, rpm_right, rpm_left):
 
        start_byte = 0xA5
        # command = 3  # VESC_COMMAND_SET_RPM
        command = 5   #VESC_COMMAND_SET_RPM_PID
        # command = 0  # VESC_COMMAND_SET_DUTY

        # Create command for right motor
        device_right = 1  # Right motor
        command_byte_right = command + (device_right << 6)
        rpm_bytes_right = struct.pack('<f', rpm_right)
        checksum_right = (start_byte + command_byte_right + sum(rpm_bytes_right)) & 0xFF
        message_right = bytearray([start_byte, command_byte_right]) + rpm_bytes_right + bytearray([checksum_right])

        # Create command for left motor
        device_left = 2  # Left motor
        command_byte_left = command + (device_left << 6)
        rpm_bytes_left = struct.pack('<f', rpm_left)
        checksum_left = (start_byte + command_byte_left + sum(rpm_bytes_left)) & 0xFF
        message_left = bytearray([start_byte, command_byte_left]) + rpm_bytes_left + bytearray([checksum_left])  
        
        if not self.dummy: 
            self.serial_conn.write(message_right)
            self.get_logger().debug(f'Sent RPM command to right motor: {rpm_right} as {message_right.hex()}')
            self.serial_conn.write(message_left)
            self.get_logger().debug(f'Sent RPM command to left motor: {rpm_left} as {message_left.hex()}')
        else:
            # Update values as no comm with ESP is established 
            self.right_motor_rpm = rpm_right
            self.left_motor_rpm = rpm_left
            self.get_logger().debug(f'Predend sent RPM command to right motor: {rpm_right} as {message_right.hex()}')
            self.get_logger().debug(f'Pretend sent RPM command to left motor: {rpm_left} as {message_left.hex()}')




    def read_serial_data(self):
        buffer = b''  # Buffer to accumulate partial reads
        self.rate = self.create_rate(100)  # Set the rate for the loop
        while True:
            if self.serial_conn.in_waiting > 0:
                data = self.serial_conn.read(self.serial_conn.in_waiting)
                buffer += data  # Add the incoming data to the buffer

                # Process the buffer for structured data
                while buffer:
                    if buffer[0] == 0xA5 and len(buffer) >= 7:  # Check for the start byte and full packet
                        packet = buffer[:7]  # Extract the 7-byte packet
                        buffer = buffer[7:]  # Remove the processed packet from the buffer

                        # Unpack the packet
                        start_byte, command, float_data, checksum = struct.unpack('<BBfB', packet)

                        # Validate checksum
                        calculated_checksum = (start_byte + command + sum(packet[2:6])) & 0xFF
                        if calculated_checksum == checksum:
                            motor = (command >> 6)  # Extract motor side (top two bits)
                            command_type = command & 0x3F  # Extract command type (lower six bits)

                            # Handle RPM data (Command 3)
                            if command_type == 3:
                                if motor == 1:  # Right motor
                                    self.right_motor_rpm = float_data
                                    #self.get_logger().info(f"Right Motor RPM: {float_data:.2f}")
                                elif motor == 2:  # Left motor
                                    self.left_motor_rpm = float_data
                                    #self.get_logger().info(f"Left Motor RPM: {float_data:.2f}")

                            # Handle Encoder Angle data (Command 40, motor 0)
                            elif command_type == 40 and motor == 0:
                                self.encoder_angle = float_data
                                #self.get_logger().info(f"Encoder Angle: {float_data:.2f}")
                        else:
                            self.get_logger().warn("Invalid checksum received")

                    else:
                        # If the buffer does not start with a valid packet, discard it
                        try:
                            debug_text = buffer.decode('utf-8', errors='ignore').replace('\x00', '').strip()
                            buffer = b''  # Clear buffer after processing
                            if debug_text:
                                self.get_logger().debug(f"Debug Message: {debug_text}")
                        except UnicodeDecodeError:
                            buffer = b''  # Clear buffer if there are decoding issues

            self.rate.sleep()

    def publish_data(self):
        # Set a rate for publishing (e.g., 10 Hz)
        publish_rate = self.create_rate(self.publish_rate_value)
        while rclpy.ok():
            # Publish the current motor velocities and encoder angle
            self.right_motor_pub.publish(Float32(data=self.right_motor_rpm))
            self.left_motor_pub.publish(Float32(data=self.left_motor_rpm))
            self.encoder_angle_pub.publish(Float32(data=self.encoder_angle))
            # self.get_logger().info(f"Published Right Motor RPM: {self.right_motor_rpm:.2f}, "
                                #    f"Left Motor RPM: {self.left_motor_rpm:.2f}, "
                                #    f"Encoder Angle: {self.encoder_angle:.2f}")
            publish_rate.sleep()
        
def main(args=None):
    rclpy.init(args=args)
    
    motor_controller_node = MotorControllerNode()
    
    try:
        rclpy.spin(motor_controller_node)
    except KeyboardInterrupt:
        pass
    finally:
        # Cleanup
        motor_controller_node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
