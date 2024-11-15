import rclpy
from rclpy.node import Node
from motor_interfaces.srv import SetMotorRpm

class MotorCommandClient(Node):
    def __init__(self):
        super().__init__('motor_command_client')
        self.client = self.create_client(SetMotorRpm, 'set_motor_rpm')

        while not self.client.wait_for_service(timeout_sec=1.0):
            self.get_logger().info('Service not available, waiting...')

    def send_rpm_command(self, rpm_right, rpm_left):
        request = SetMotorRpm.Request()
        request.command.right = rpm_right
        request.command.left = rpm_left

        future = self.client.call_async(request)
        rclpy.spin_until_future_complete(self, future)

        if future.result() is not None:
            self.get_logger().info(f"Service call success: {future.result().success}")
        else:
            self.get_logger().error("Service call failed")

def main(args=None):
    rclpy.init(args=args)
    motor_command_client = MotorCommandClient()

    motor_command_client.send_rpm_command(1500.0, 1500.0)

    motor_command_client.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
