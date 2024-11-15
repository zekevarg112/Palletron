#ifndef VESC_UTILS_H
#define VESC_UTILS_H
#include "../include.h"

#ifdef VESC_DEBUG 
#define VESC_DEBUG_PRINT(format, ...) debug_message("VESC: Debug: " , format, ##__VA_ARGS__);
#else
#define VESC_DEBUG_PRINT(format, ...)
#endif

// helper functions for sending data to Vesc, 
// documentation here: https://github.com/vedderb/bldc/blob/master/documentation/comm_can.md



// Scaling factors
#define VESC_DUTY_SCALE                100000UL
#define VESC_CURRENT_SCALE             1000UL
#define VESC_CURRENT_BRAKE_SCALE       1000UL
#define VESC_RPM_SCALE                 1UL
#define VESC_POS_SCALE                 1000000UL
#define VESC_CURRENT_REL_SCALE         100000UL
#define VESC_CURRENT_BRAKE_REL_SCALE   100000UL
#define VESC_CURRENT_HANDBRAKE_SCALE   1000UL
#define VESC_CURRENT_HANDBRAKE_REL_SCALE 100000UL

#define RIGHT 0
#define LEFT 1


#ifdef ENABLE_PI_CONTROL
#include "PIcontroller.h"

class Vesc{
    public:
        // constructor, initiates and handles vesc communication
        Vesc();
        void init();
        float get_right_rpm();
        float get_left_rpm();


        // function to setup vesc device with empty data
        void setup_vesc(CAN_Device* device, uint8_t vesc_id);


        void setup_monitor(bool motor, uint8_t command_id);

        // Base function, returns CAN_Data object set up for vesc communication
        void update_vesc_data(bool motor, uint8_t command, int32_t data);

        // set duty cycle. -1.0 to 1.0
        void vesc_set_duty(bool motor, float duty);

        // set currunt -Motor_max to Motor_max
        void vesc_set_current(bool motor, float duty);

        // set mechanical rpm -Motor_max_rpm to Motor_max_rpm
        void vesc_set_rpm(bool motor, float duty);

        // set erpm -Motor_max_erpm to Motor_emax_rpm
        void vesc_set_Erpm(bool motor, float duty);


        void vesc_handle_command(bool motor, uint8_t command, float data);

    

        TaskHandle_t _callback_function_handle;
        static void pid_update_thread_task(void* param);
        void pid_update_thread_function();

    
        // thread for continiously sending vesc commands
        TaskHandle_t _vesc_comms_handle;
        static void vesc_comms_task(void* param);
        void vesc_comms_function();

        // bit 0 - pid mode right
        // bit 1 - pid mode left
        EventGroupHandle_t _controller_mode;
        
        PIController right_controller;
        PIController left_controller;





};





#else

// function to setup vesc device with empty data
void setup_vesc(CAN_Device* device, uint8_t vesc_id);

// Base function, returns CAN_Data object set up for vesc communication
void update_vesc_data(CAN_Device* device, uint8_t command, uint32_t data);

// set duty cycle. -1.0 to 1.0
void vesc_set_duty(CAN_Device* device, float duty);

// set currunt -Motor_max to Motor_max
void vesc_set_current(CAN_Device* device, float duty);

// set mechanical rpm -Motor_max_rpm to Motor_max_rpm
void vesc_set_rpm(CAN_Device* device, float duty);

// set erpm -Motor_max_erpm to Motor_emax_rpm
void vesc_set_Erpm(CAN_Device* device, float duty);

// set motor position 0.0-360.0
void vesc_set_pos(CAN_Device* device, float duty);

void vesc_handle_command(CAN_Device* device, uint8_t command, float data);

void setup_monitor(CAN_Manager* CAN, CAN_Device* device, uint8_t command_id);

#endif
#endif