#include "Vesc_utils.h"


#ifdef ENABLE_PI_CONTROL

Vesc::Vesc():
    right_controller(VESC_PID_RIGHT_KP, VESC_PID_RIGHT_KI, -1.0, 1.0, VESC_PID_RATE_LIMIT),
    left_controller(VESC_PID_LEFT_KP, VESC_PID_LEFT_KI, -1.0, 1.0, VESC_PID_RATE_LIMIT),
    _controller_mode(nullptr), _vesc_comms_handle(nullptr), _callback_function_handle(nullptr)
    {
}

void Vesc::init(){
    // initialize vesc nodes
    setup_vesc(&datalayer.Motor_R, VESC_ID_RIGHT);
    vesc_set_duty(RIGHT, 0.0);
    setup_vesc(&datalayer.Motor_L, VESC_ID_LEFT);
    vesc_set_duty(LEFT, 0.0);

    setup_monitor(RIGHT, VESC_STATUS_PACKET_1);
    setup_monitor(LEFT, VESC_STATUS_PACKET_1);

    // Initialize RTOS objects
    while (1) {  // do not exit this loop until all RTOS objects are initialized
        _controller_mode = xEventGroupCreate();

        // check if all are initialized correctly and break out of loop.
        if (_controller_mode != nullptr) break;
        CAN_DEBUG_PRINT("Could not initiate all rtos components, retrying.\n");
    }

    xTaskCreatePinnedToCore(&vesc_comms_task, "Comms", 1024*4, this, 2, &_vesc_comms_handle, 1); // start can publish thread 
    xTaskCreatePinnedToCore(&pid_update_thread_task, "PID", 1024*4, this, 2, &_callback_function_handle, 1); // start pid thread 


}

void Vesc::setup_vesc(CAN_Device* device, uint8_t vesc_id){
    device->node_id = vesc_id;
    device->mutex = xSemaphoreCreateBinary();
    // set data to an empty un-initialized data object
    CAN_Data data = {
        .type = {.message_type = MESSAGE_TYPE_VESC},
        .initialized = false,
        .parent = device
        };
    device->data = std::make_shared<CAN_Data>(data); // assign to device´s data parameter
    xSemaphoreGive(device->mutex);
    VESC_DEBUG_PRINT("Setup id:%d\n", datalayer.Motor_R.node_id);

}

void Vesc::setup_monitor(bool motor, uint8_t command_id){
    CAN_Device* device;
    if (motor == LEFT) device = &datalayer.Motor_L;
    else device = &datalayer.Motor_R;
    VESC_status_data data = {.data={.status1={.erpm=0,.current=0,.dutyCycle=0}}};
    CAN_Data new_monitor = {
        .type = {
            .message_type = MESSAGE_TYPE_VESC_STATUS
        },
        .initialized = true,
        .parent = device,
        .data = data,
        .command = command_id
    };
    std::shared_ptr<CAN_Data> new_ptr = std::make_shared<CAN_Data>(new_monitor);
    CAN.Monitored_datas.push_back(new_ptr);
}

void Vesc::update_vesc_data(bool motor, uint8_t command, int32_t data) {
    CAN_Device* device;
    if (motor == LEFT) device = &datalayer.Motor_L;
    else device = &datalayer.Motor_R;

    VESC_DEBUG_PRINT("Update data for id:%d\n", device->node_id);

    xSemaphoreTake(device->mutex, portMAX_DELAY); // always take mutex before modifying or reading from any device
    device->data->type.message_type = MESSAGE_TYPE_VESC; //make sure message type is correct
    device->data->initialized = true; //make data initialized
    device->data->data = data;
    device->data->command = command;
    xSemaphoreGive(device->mutex);
}

void Vesc::vesc_set_duty(bool motor, float duty){
    
    int32_t data = (int32_t)round(duty*VESC_DUTY_SCALE); // scale by scaling factor, round and convert to integer
    VESC_DEBUG_PRINT("Set duty to: %f\n", duty);
    update_vesc_data(motor, VESC_COMMAND_SET_DUTY, data);
}

void Vesc::vesc_set_current(bool motor, float current){
    int32_t data = (int32_t)round(current*VESC_CURRENT_SCALE); // scale by scaling factor, round and convert to integer
    VESC_DEBUG_PRINT("Set current to: %f\n", current);
    update_vesc_data(motor, VESC_COMMAND_SET_CURRENT, data);
}

void Vesc::vesc_set_rpm(bool motor, float rpm){
    VESC_DEBUG_PRINT("Set rpm to: %f\n", rpm);
    vesc_set_Erpm(motor, rpm*VESC_MOTOR_POLES); // mechanical rpm cannot be set directly, instead use Erpm = poles * rpm
}

void Vesc::vesc_set_Erpm(bool motor, float erpm){
    int32_t data = (int32_t)round(erpm*VESC_RPM_SCALE); // scale by scaling factor, round and convert to integer
    VESC_DEBUG_PRINT("Set Erpm to: %f\n", erpm);
    update_vesc_data(motor, VESC_COMMAND_SET_RPM, data);
}



void Vesc::vesc_handle_command(bool motor, uint8_t command, float data){
    if(command == VESC_COMMAND_SET_PID_RPM){
        VESC_DEBUG_PRINT("PID mode\n")

        uint8_t current_mode = xEventGroupGetBits(_controller_mode);
        // new setpoint
        if(motor == RIGHT) {
            // if controller was not already on, reset
            if(current_mode & 0b1<<RIGHT != 0) right_controller.reset();
            right_controller.setpoint(data);
        }
        else {
            // if controller was not already on, reset
            if(current_mode & 0b1<<LEFT != 0) left_controller.reset();
            left_controller.setpoint(data); 
        }

        xEventGroupSetBits(_controller_mode, 0b1<<motor); // turn on corresponding pid controller

    }
    // if going back to manual mode
    else {
        // turn off corresponding pid controller
        xEventGroupClearBits(_controller_mode, 0b1<<motor);
        switch (command)
        {
        case VESC_COMMAND_SET_DUTY:
            VESC_DEBUG_PRINT("Duty mode\n");
            vesc_set_duty(motor, data);
            break;

        case VESC_COMMAND_SET_CURRENT:
            vesc_set_current(motor, data);
            break;

        case VESC_COMMAND_SET_CURRENT_BRAKE:
            VESC_DEBUG_PRINT("Set brake current not available!\n");
            break;

        case VESC_COMMAND_SET_RPM:
            vesc_set_rpm(motor, data);
            break;

        case VESC_COMMAND_SET_POS:
            VESC_DEBUG_PRINT("Set pos not available!\n");
            break;

        case VESC_COMMAND_SET_CURRENT_REL:
            VESC_DEBUG_PRINT("Set current rel not available!\n");
            break;

        case VESC_COMMAND_SET_CURRENT_BRAKE_REL:
            VESC_DEBUG_PRINT("Set brake current rel not available!\n");
            break;

        case VESC_COMMAND_SET_CURRENT_HANDBRAKE:
            VESC_DEBUG_PRINT("Set handbrake current not available!\n");
            break;

        case VESC_COMMAND_SET_CURRENT_HANDBRAKE_REL:
            VESC_DEBUG_PRINT("Set handbrake current rel not available!\n");
            break;
        
        default:
            VESC_DEBUG_PRINT("Unknown commad: %d\n", command);
            break;
        }
    }
}



void Vesc::pid_update_thread_task(void* param){
    Vesc* vesc = static_cast<Vesc*>(param);
    vesc->pid_update_thread_function();

    
}

void Vesc::pid_update_thread_function(){
    float last_duty_right = 0.0;
    float last_duty_left = 0.0;
    while(1){
        // Disable controllers when not running
        // if(xEventGroupGetBits(_controller_mode) & 0b11 != 0){ // if pid mode off for both
        //     xEventGroupWaitBits(_controller_mode, 0x1, false, true, portMAX_DELAY); // wait for pid to turn on
        //     // reset controllers
        //     right_controller.reset();
        //     left_controller.reset();
        // }
        //wait for new data to arrive
        uint8_t current_mode = xEventGroupGetBits(_controller_mode);
        xEventGroupWaitBits(CAN.CAN_status_flags, CAN_FLAG_NEW_MONITORED_AVAILABLE, true, true, portMAX_DELAY);
        for (const auto& data_ptr : CAN.Monitored_datas){
            if(data_ptr->updated){ // look at data if its been updated
                if(data_ptr->parent->node_id == VESC_ID_RIGHT){ // if data received for right motor
                    if(current_mode & 0b1<<RIGHT != 0){ // if the right controller is on
                        float new_val = right_controller.update(float(std::get<VESC_status_data>(data_ptr->data).data.status1.erpm) / VESC_MOTOR_POLES);
                        //printf("right new pi val: %f\n", new_val);
                        if( new_val != last_duty_right){
                            vesc_set_duty(RIGHT, new_val);
                            last_duty_right = new_val;
                        }
                        // update duty value with pid output
                    }
                }
                else if(data_ptr->parent->node_id == VESC_ID_LEFT){ // if data received for left motor
                    if(current_mode & 0b1<<LEFT != 0){ // if left controller is on
                        // update duty value with pid output
                        float new_val = left_controller.update(float(std::get<VESC_status_data>(data_ptr->data).data.status1.erpm) / VESC_MOTOR_POLES);
                        if( new_val != last_duty_left){
                            vesc_set_duty(LEFT, new_val);
                            last_duty_left = new_val;
                        }      
                    }
                }
                data_ptr->updated=false;
            }
        }
        

    }
}


void Vesc::vesc_comms_task(void* param){
    Vesc* vesc = static_cast<Vesc*>(param);
    vesc->vesc_comms_function();    
}

void Vesc::vesc_comms_function(){
    uint8_t task_delay = 1000/VESC_COMMAND_FREQUENCY; // time between commands

    while(1){
        // send to right motor
        if(xSemaphoreTake(datalayer.Motor_R.mutex, pdMS_TO_TICKS(10)) == pdTRUE){
            CAN_Data* ptr = datalayer.Motor_R.data.get();
            xQueueSendToBack(CAN.message_send_queue, &ptr, pdMS_TO_TICKS(10));
            xSemaphoreGive(datalayer.Motor_R.mutex);
        } else VESC_DEBUG_PRINT("Could not get mutex for right motor\n");

        // send to left motor
        if(xSemaphoreTake(datalayer.Motor_L.mutex, pdMS_TO_TICKS(10)) == pdTRUE){
            CAN_Data* ptr = datalayer.Motor_L.data.get();
            xQueueSendToBack(CAN.message_send_queue, &ptr, pdMS_TO_TICKS(10));
            xSemaphoreGive(datalayer.Motor_L.mutex);
        } else VESC_DEBUG_PRINT("Could not get mutex for left motor\n");

        vTaskDelay(pdMS_TO_TICKS(task_delay)); // wait until next publish, configured in config.h
    }

}
















#else
// Old stuff

void setup_vesc(CAN_Device* device, uint8_t vesc_id){
    device->node_id = vesc_id;
    device->mutex = xSemaphoreCreateBinary();
    int32_t temp = 0;
    // set data to an empty un-initialized data object
    CAN_Data data = {
        .type = {.message_type = MESSAGE_TYPE_VESC},
        .initialized = false,
        .parent = device
        };
    device->data = std::make_shared<CAN_Data>(data); // assign to device´s data parameter
    xSemaphoreGive(device->mutex);
    vesc_set_rpm(device, 0.0);
}

void update_vesc_data(CAN_Device* device, uint8_t command, int32_t data) {

    xSemaphoreTake(device->mutex, portMAX_DELAY); // always take mutex before modifying or reading from any device
    device->data->type.message_type = MESSAGE_TYPE_VESC; //make sure message type is correct
    device->data->initialized = true; //make data initialized
    device->data->data = data;
    device->data->command = command;
    xSemaphoreGive(device->mutex);
}

void vesc_set_duty(CAN_Device* device, float duty){
    int32_t data = (int32_t)round(duty*VESC_DUTY_SCALE); // scale by scaling factor, round and convert to integer
    VESC_DEBUG_PRINT("Set duty to: %f\n", duty);
    update_vesc_data(device, VESC_COMMAND_SET_DUTY, data);
}


void vesc_set_current(CAN_Device* device, float current){
    int32_t data = (int32_t)round(current*VESC_CURRENT_SCALE); // scale by scaling factor, round and convert to integer
    VESC_DEBUG_PRINT("Set current to: %f\n", current);
    update_vesc_data(device, VESC_COMMAND_SET_CURRENT, data);
}

void vesc_set_rpm(CAN_Device* device, float rpm){
    VESC_DEBUG_PRINT("Set rpm to: %f\n", rpm);
    vesc_set_Erpm(device, rpm*VESC_MOTOR_POLES); // mechanical rpm cannot be set directly, instead use Erpm = poles * rpm
}

void vesc_set_Erpm(CAN_Device* device, float erpm){
    int32_t data = (int32_t)round(erpm*VESC_RPM_SCALE); // scale by scaling factor, round and convert to integer
    VESC_DEBUG_PRINT("Set Erpm to: %f\n", erpm);
    update_vesc_data(device, VESC_COMMAND_SET_RPM, data);
}


void vesc_handle_command(CAN_Device* device, uint8_t command, float data){
    switch (command)
    {
    case VESC_COMMAND_SET_DUTY:
        vesc_set_duty(device, data);
        break;

    case VESC_COMMAND_SET_CURRENT:
        vesc_set_current(device, data);
        break;

    case VESC_COMMAND_SET_CURRENT_BRAKE:
        VESC_DEBUG_PRINT("Set brake current not available!\n");
        break;

    case VESC_COMMAND_SET_RPM:
        vesc_set_rpm(device, data);
        break;

    case VESC_COMMAND_SET_POS:
        VESC_DEBUG_PRINT("Set pos not available!\n");
        break;

    case VESC_COMMAND_SET_CURRENT_REL:
        VESC_DEBUG_PRINT("Set current rel not available!\n");
        break;

    case VESC_COMMAND_SET_CURRENT_BRAKE_REL:
        VESC_DEBUG_PRINT("Set brake current rel not available!\n");
        break;

    case VESC_COMMAND_SET_CURRENT_HANDBRAKE:
        VESC_DEBUG_PRINT("Set handbrake current not available!\n");
        break;

    case VESC_COMMAND_SET_CURRENT_HANDBRAKE_REL:
        VESC_DEBUG_PRINT("Set handbrake current rel not available!\n");
        break;
    
    default:
        VESC_DEBUG_PRINT("Unknown commad: %d\n", command);
        break;
    }
}

void setup_monitor(CAN_Manager* CAN, CAN_Device* device, uint8_t command_id){
    VESC_status_data data = {.data={.status1={.erpm=0,.current=0,.dutyCycle=0}}};
    CAN_Data new_monitor = {
        .type = {
            .message_type = MESSAGE_TYPE_VESC_STATUS
        },
        .initialized = true,
        .parent = device,
        .data = data,
        .command = command_id
    };
    std::shared_ptr<CAN_Data> new_ptr = std::make_shared<CAN_Data>(new_monitor);
    CAN->Monitored_datas.push_back(new_ptr);
}

#endif