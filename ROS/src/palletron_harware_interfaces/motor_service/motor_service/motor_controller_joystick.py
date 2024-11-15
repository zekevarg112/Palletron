import rclpy
from rclpy.node import Node
from motor_interfaces.srv import SetMotorRpm
from sensor_msgs.msg import Joy

class TeleopToMotorNode(Node):
    def __init__(self):
        super().__init__('teleop_to_motor_node')
        
        # Parameters
        self.declare_parameter('max_rpm', 100.0)  # Distance between the wheels in meters

        
        # Get parameters
        self.max_rpm = self.get_parameter('max_rpm').get_parameter_value().double_value
        self.last_left = 0
        self.last_right = 0

        # Subscribe to the /cmd_vel topic
        self.subscription = self.create_subscription(
            Joy,
            'joy',
            self.joy_callback,
            10)
        
        # Create a client for the set_motor_rpm service
        self.client = self.create_client(SetMotorRpm, 'set_motor_rpm')
        while not self.client.wait_for_service(timeout_sec=10.0):
            self.get_logger().info('Waiting for set_motor_rpm service...')
        self.get_logger().info('Started.')


    def joy_callback(self, joy: Joy):
        # Convert Twist message to wheel RPMs
        left_axis = joy.axes[1]  # Forward velocity in m/s
        right_axis = joy.axes[4]  # Rotational velocity in rad/s
        if(abs(left_axis-self.last_left) > 0.05 or abs(right_axis-self.last_right) > 0.05):

            rpm_left = 0
            rpm_right = 0
            self.get_logger().info(f'Right: {right_axis}, Left: {left_axis}')
            # Convert wheel velocities to RPM
            # rpm_right = right_axis*self.max_rpm
            # rpm_left = left_axis*self.max_rpm
            rpm_right = right_axis/3
            rpm_left = left_axis/3
            
            self.last_left = left_axis
            self.last_right = right_axis

            # Send RPM values to motor controller via service
            
            self.send_motor_command(rpm_right, rpm_left)

    def send_motor_command(self, rpm_right, rpm_left):
        # Create request for the set_motor_rpm service
        request = SetMotorRpm.Request()
        request.command.right = float(rpm_right)
        request.command.left = float(rpm_left)

        # Send request asynchronously
        future = self.client.call_async(request)
        future.add_done_callback(self.service_callback)

    def service_callback(self, future):
        try:
            response = future.result()
            if response.success:
                pass
                # self.get_logger().info('Motor RPM command sent successfully')
            else:
                self.get_logger().error('Failed to send Motor RPM command')
        except Exception as e:
            self.get_logger().error(f'Service call failed: {e}')

def main(args=None):
    rclpy.init(args=args)
    
    teleop_to_motor_node = TeleopToMotorNode()
    
    try:
        rclpy.spin(teleop_to_motor_node)
    except KeyboardInterrupt:
        pass
    finally:
        # Cleanup
        teleop_to_motor_node.destroy_node()
        rclpy.shutdown()

if __name__ == '__main__':
    main()
