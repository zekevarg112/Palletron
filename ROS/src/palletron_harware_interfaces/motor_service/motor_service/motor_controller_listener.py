import rclpy
from rclpy.node import Node
from geometry_msgs.msg import Twist
from motor_interfaces.srv import SetMotorRpm

class TeleopToMotorNode(Node):
    def __init__(self):
        super().__init__('teleop_to_motor_node')
        
        # Parameters
        self.declare_parameter('wheel_base', 0.5)  # Distance between the wheels in meters
        self.declare_parameter('wheel_radius', 0.1)  # Radius of the wheels in meters
        
        # Get parameters
        self.wheel_base = self.get_parameter('wheel_base').get_parameter_value().double_value
        self.wheel_radius = self.get_parameter('wheel_radius').get_parameter_value().double_value

        # Subscribe to the /cmd_vel topic
        self.subscription = self.create_subscription(
            Twist,
            'cmd_vel',
            self.cmd_vel_callback,
            10)
        
        # Create a client for the set_motor_rpm service
        self.client = self.create_client(SetMotorRpm, 'set_motor_rpm')
        while not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Waiting for set_motor_rpm service...')

    def cmd_vel_callback(self, msg: Twist):
        # Convert Twist message to wheel RPMs
        linear_velocity = msg.linear.x  # Forward velocity in m/s
        angular_velocity = msg.angular.z  # Rotational velocity in rad/s

        # Calculate wheel velocities (m/s)
        v_right = linear_velocity + (self.wheel_base / 2.0) * angular_velocity
        v_left = linear_velocity - (self.wheel_base / 2.0) * angular_velocity

        # Convert wheel velocities to RPM
        rpm_right = (v_right / (2 * 3.14159 * self.wheel_radius)) * 60.0
        rpm_left = (v_left / (2 * 3.14159 * self.wheel_radius)) * 60.0

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
                self.get_logger().info('Motor RPM command sent successfully')
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
