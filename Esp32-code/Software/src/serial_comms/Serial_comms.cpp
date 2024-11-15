#include "serial_comms.h"

xSemaphoreHandle serial_mutex;

serial_parsed_data check_and_parse_serial(){
    if (Serial.available() >= 7) {  // We expect 7 bytes: start byte  + command byte + 4 bytes of float + checksum
        uint8_t start_byte = Serial.read();

        if (start_byte == 0xA5) {  // Check for the start byte
            uint8_t command_byte = Serial.read(); // the command byte

            uint8_t command = command_byte & (~(0b11 << 6)); // command is bottom 6 bytes, max command is then 63
            uint8_t device = (command_byte & (0b11 << 6)) >> 6; // device is top 2 bytes
            
            uint8_t data[4];
            for (int i = 0; i < 4; i++) {
                data[i] = Serial.read();  // Read the 4 bytes for the float
            }

            uint8_t received_checksum = Serial.read();  // Read the checksum byte

            // Calculate the checksum (sum of startByte, packetType, and the data bytes)
            uint8_t calculated_checksum = (start_byte + command_byte + data[0] + data[1] + data[2] + data[3]) & 0xFF;

            if (calculated_checksum == received_checksum) {
                // Reassemble the float from the 4 bytes
                float received_float;
                memcpy(&received_float, data, sizeof(received_float));  // Copy the byte array into the float

                // Print the received float
                SERIAL_PARSER_DEBUG_PRINT("Received: Device: %d, Command: %d, Data: %f\n", device, command, received_float);
                return serial_parsed_data{
                    .succes = true,
                    .device = device,
                    .command = command,
                    .data = received_float};
            } else {
                // Handle checksum error
                SERIAL_PARSER_DEBUG_PRINT("Checksum error: Packet corrupted.\n");
                return serial_parsed_data{.succes=false};
            }
            
        } else if (start_byte == 0xA6) {  // Handshake byte (0xA6)
               
            // Respond to the handshake with the device identifier
            xSemaphoreTake(serial_mutex, portMAX_DELAY);
            Serial.write(0xA6);  // Replace with your device identifier
            xSemaphoreGive(serial_mutex);
            SERIAL_PARSER_DEBUG_PRINT("Responded to handshake\n");
            return serial_parsed_data{.succes = true};  // Optionally return some data here
            
        } 
        else {
            // handle incorrect start byte
            SERIAL_PARSER_DEBUG_PRINT("Incorrect start byte, ignoring.\n");
            return serial_parsed_data{.succes=false};

        }
    }
    return serial_parsed_data{.succes=false};
}

void send_vesc_rpm(CAN_Device* device, bool side){
    //if (!serial_enabled) return;
    for (const auto& data_ptr : CAN.Monitored_datas){
        if(xSemaphoreTake(data_ptr->parent->mutex, pdMS_TO_TICKS(50))){
            if(data_ptr->parent == device &&
                data_ptr->command==VESC_STATUS_PACKET_1) {


                float rpm = float(std::get<VESC_status_data>(data_ptr->data).data.status1.erpm) / VESC_MOTOR_POLES;

                uint8_t data_bytes[4];
                std::memcpy(data_bytes, &rpm, sizeof(rpm));

                uint8_t device_side;
                if (side == RIGHT) device_side = 1;
                else device_side = 2;

                serial_rx_data data_to_send = {
                    .data = {
                        .start_byte = 0xA5,
                        .command = (uint8_t)(SERIAL_COMMAND_RPM + (device_side << 6)),
                        .u8_float_data = {data_bytes[0], data_bytes[1], data_bytes[2], data_bytes[3]}
                    }
                };


                uint8_t checksum = 0;
                for (int i = 0; i < 6;i++){
                    checksum += data_to_send.u8[i];
                }
                checksum = checksum & 0xFF;
                data_to_send.data.checksum = checksum;


                xSemaphoreTake(serial_mutex, portMAX_DELAY);
                for(int i = 0; i < 7; i++){                  
                    Serial.write(data_to_send.u8[i]);
                }
                xSemaphoreGive(serial_mutex);
            }
            xSemaphoreGive(data_ptr->parent->mutex);
        }
        else SERIAL_PARSER_DEBUG_PRINT("Could not get mutex!");

    }
}

// sent as command 
void send_encoder_angle(float angle){
    //if (!serial_enabled) return;
    uint8_t data_bytes[4];
    std::memcpy(data_bytes, &angle, sizeof(angle));

    uint8_t device_side = 0;

    serial_rx_data data_to_send = {
        .data = {
            .start_byte = 0xA5,
            .command = (uint8_t)(SERIAL_COMMAND_ENCODER + (device_side << 6)),
            .u8_float_data = {data_bytes[0], data_bytes[1], data_bytes[2], data_bytes[3]}
        }
    };


    uint8_t checksum = 0;
    for (int i = 0; i < 6;i++){
        checksum += data_to_send.u8[i];
    }
    checksum = checksum & 0xFF;
    data_to_send.data.checksum = checksum;


    xSemaphoreTake(serial_mutex, portMAX_DELAY);
    for(int i = 0; i < 7; i++){                  
        Serial.write(data_to_send.u8[i]);
    }
    xSemaphoreGive(serial_mutex);
}
