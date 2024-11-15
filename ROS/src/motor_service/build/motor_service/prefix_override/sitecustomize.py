import sys
if sys.prefix == '/usr':
    sys.real_prefix = sys.prefix
    sys.prefix = sys.exec_prefix = '/home/viktor/Desktop/palletron_harware_interfaces-master/motor_service/install/motor_service'
