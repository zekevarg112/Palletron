from setuptools import setup
import os
from glob import glob

package_name = 'motor_service'

setup(
    name=package_name,
    version='0.0.0',
    packages=[package_name],
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
        (os.path.join('share', package_name, 'launch'), glob(os.path.join('launch', '*launch.[pxy][yma]*'))),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='kasper',
    maintainer_email='kasper.georen@gmail.com',
    description='TODO: Package description',
    license='TODO: License declaration',
    tests_require=['pytest'],
    entry_points={
    'console_scripts': [
        'motor_controller_service = motor_service.motor_controller_service:main',
        'motor_client = motor_service.motor_controller_client:main',
        'motor_controller_listener = motor_service.motor_controller_listener:main',
        'motor_controller_joystick = motor_service.motor_controller_joystick:main',
        'rumble_test = motor_service.rumble_test:main',
        'failsafe_node = motor_service.failsafe_node:main',
    
        ],
    },
)
