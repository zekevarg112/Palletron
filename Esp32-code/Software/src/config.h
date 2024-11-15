#ifndef CONFIG_H
#define CONFIG_H
#include <Arduino.h>

// In this file all user configurable parameters are set

#define ENABLE_PI_CONTROL

//enable/disable debugging and error via Serial
#define CAN_ERROR_DEBUG
//#define CAN_DEBUG
#define VESC_DEBUG
#define SERIAL_PARSER_DEBUG

#define SDA_PIN 25
#define SCL_PIN 33

// CAN parameters
#define VESC_ID_RIGHT 62
#define VESC_ID_LEFT 63


#define VESC_STATUS_FREQUENCY 10 // Hz
#define VESC_COMMAND_FREQUENCY 40 // Hz

#define VESC_MOTOR_POLES 20 // amount of poles for the motors (40 magnets = 20 poles?)


// Vesc pid parameters
#define VESC_PID_RIGHT_KP 0.0002f
#define VESC_PID_RIGHT_KI 0.00002f
#define VESC_PID_RIGHT_KD 0.0f // not used
#define VESC_PID_LEFT_KP 0.0002f
#define VESC_PID_LEFT_KI 0.00002f
#define VESC_PID_LEFT_KD 0.0f // not used
#define VESC_PID_RATE_LIMIT 0.005f // duty/ms




//status led brightness (0-255)
#define LED_BRIGHTNESS 40

//time to wait for CAN response before timing out, in ms
#define CAN_MAX_WAIT_TIME 10
//interval between device scans in ms
#define SCAN_DEVICES_INTERVAL 5000
//interval between updates for devices configured with "update:"low", in ms
#define LOW_UPDATE_INTERVAL 1000
//interval between updates for devices configured with "update:"high", in ms
#define HIGH_UPDATE_INTERVAL 100
//time to wait for response before device is considered inactive, in ms
#define TIME_TO_WAIT_INACTIVE 2000


#endif