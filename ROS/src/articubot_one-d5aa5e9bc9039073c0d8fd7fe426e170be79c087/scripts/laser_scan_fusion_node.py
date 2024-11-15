#!/usr/bin/env python3

import rclpy
from rclpy.node import Node
from sensor_msgs.msg import LaserScan
import message_filters
import math
from rclpy.qos import QoSProfile, ReliabilityPolicy, HistoryPolicy

class LaserScanFusionNode(Node):
    def __init__(self):
        super().__init__('laser_scan_fusion_node')

        # Define a QoS profile with BEST_EFFORT reliability
        qos_profile = QoSProfile(
            reliability=ReliabilityPolicy.BEST_EFFORT,
            history=HistoryPolicy.KEEP_LAST,
            depth=10
        )

        # Subscribers for the two input LaserScan topics with the BEST_EFFORT QoS profile
        self.sub_low = message_filters.Subscriber(self, LaserScan, '/velodyne_scan_low', qos_profile=qos_profile)
        self.sub_long = message_filters.Subscriber(self, LaserScan, '/velodyne_scan_long', qos_profile=qos_profile)

        # Publisher for the fused LaserScan topic
        self.pub = self.create_publisher(LaserScan, '/velodyne_scan_fusion', qos_profile)

        # Synchronize the two LaserScan messages
        self.ts = message_filters.ApproximateTimeSynchronizer(
            [self.sub_low, self.sub_long], queue_size=10, slop=0.1
        )
        self.ts.registerCallback(self.scan_callback)

    def scan_callback(self, scan_low, scan_long):
        # Create a new LaserScan message
        fused_scan = LaserScan()
        fused_scan.header = scan_low.header  # Use header from one of the scans

        # Assume scans have the same parameters
        fused_scan.angle_min = scan_low.angle_min
        fused_scan.angle_max = scan_low.angle_max
        fused_scan.angle_increment = scan_low.angle_increment
        fused_scan.time_increment = scan_low.time_increment
        fused_scan.scan_time = scan_low.scan_time
        fused_scan.range_min = scan_low.range_min
        fused_scan.range_max = scan_long.range_max

        # Initialize the fused ranges array
        fused_ranges = []
        for low_range, long_range in zip(scan_low.ranges, scan_long.ranges):
            if not math.isinf(low_range):
                fused_ranges.append(low_range)
            else:
                fused_ranges.append(long_range)
        fused_scan.ranges = fused_ranges

        # (Optional) Handle intensities if needed
        fused_scan.intensities = scan_low.intensities  # Or combine as appropriate

        # Publish the fused scan
        self.pub.publish(fused_scan)

def main(args=None):
    rclpy.init(args=args)
    node = LaserScanFusionNode()
    rclpy.spin(node)
    node.destroy_node()
    rclpy.shutdown()

if __name__ == '__main__':
    main()
