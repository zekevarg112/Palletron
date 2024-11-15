#include "CANOpenDriver.h"

#define CAN_TX_PIN 26
#define CAN_RX_PIN 27
#define CAN_SE_PIN 23


const uint8_t rx_queue_size = 50;  // Receive Queue size
const uint8_t tx_queue_size = 50;  // Receive Queue size

// CAN Config
CAN_device_t CAN_cfg;

/*===============================
          Utils
================================*/

uint32_t little_to_big_endian(uint32_t little_endian) {
    return ((little_endian >> 24) & 0x000000FF) |  // Move byte 3 to byte 0
           ((little_endian >> 8)  & 0x0000FF00) |  // Move byte 2 to byte 1
           ((little_endian << 8)  & 0x00FF0000) |  // Move byte 1 to byte 2
           ((little_endian << 24) & 0xFF000000);   // Move byte 0 to byte 3
}


/*===============================
          Update helpers
================================*/

void CAN_Manager::update_number(CAN_Data* data_to_update) {
  // check data is actually of number type, to avoid corruption
  if (data_to_update->type.datatype != DATATYPE_NUMBER) return;

  // request
  if (request_data(data_to_update) != CAN_TX_SUCCESS) return;

  // receive
  CAN_frame_t received_frame;
  if (receive_request_response(data_to_update, &received_frame) !=
      CAN_RX_SUCCESS)
    return;

  // store
  write_value(data_to_update, &received_frame);
}

void CAN_Manager::update_string(CAN_Data* data_to_update) {
  // check data is actually of string type, to avoid corruption
  if (data_to_update->type.datatype != DATATYPE_STRING) return;

  // request init transfer
  if (request_data(data_to_update) != CAN_TX_SUCCESS) return;

  // receive string length
  CAN_frame_t rx_frame;
  if (receive_request_response(data_to_update, &rx_frame) != CAN_RX_SUCCESS)
    return;

  uint32_t string_length =
      static_cast<uint32_t>(rx_frame.data.SDO.SDO_data.u8[0]) |
      (static_cast<uint32_t>(rx_frame.data.SDO.SDO_data.u8[1]) << 8) |
      (static_cast<uint32_t>(rx_frame.data.SDO.SDO_data.u8[2]) << 16) |
      (static_cast<uint32_t>(rx_frame.data.SDO.SDO_data.u8[3]) << 24);

  // initiate toggle bit to 0, first message. After that it should toggle
  // between messages
  bool toggle = 0;
  String received_string;  // empty string to store in
  int status;
  do {
    if (request_next_segment(data_to_update, toggle) != CAN_TX_SUCCESS) return;
    status = receive_string_segment(data_to_update, toggle, received_string);
    if (status == CAN_RX_ERROR) return;  // abort if fail
    toggle = !toggle;                    // toggle toggle-bit
  } while (status != CAN_RX_END_OF_SEGMENT);  // stop when end of segment

  // sanity check result
  if (received_string.length() != string_length) {
    CAN_ERROR_PRINT("Received string length(%d) does not match expected(%d)!\n",
                    received_string.length(), string_length);
  } else {
    // store string
    data_to_update->data = received_string;
    CAN_DEBUG_PRINT("Updated String to: %s\n", received_string.c_str());
  }
}

void CAN_Manager::update_bitfield(CAN_Data* data_to_update) {
  // check data is actually of bitfield type, to avoid corruption
  if (data_to_update->type.datatype != DATATYPE_BITFEILD) return;

  // request data
  if (request_data(data_to_update) != CAN_TX_SUCCESS) return;

  // receive data
  CAN_frame_t received_frame;
  if (receive_request_response(data_to_update, &received_frame) !=
      CAN_RX_SUCCESS)
    return;

  // store
  write_value(data_to_update, &received_frame);
}

/*===============================
      Transmit functions
================================*/

int CAN_Manager::sdo_request_data(uint8_t node_id, uint16_t index,
                              uint8_t sub_index) {
  CAN_DEBUG_PRINT("Requesting data for node with id %d.\n", node_id);
  CAN_DEBUG_PRINT("\t Requesting: ");

  // calculate canopen id
  uint32_t COB_id = 0x600L + node_id;

  // construct frame. Command 0x40 - Read request
  CAN_frame_t frame = {.FIR = {.B =
                                   {
                                       .DLC = 8,
                                       .FF = CAN_frame_std,
                                   }},
                       .MsgID = COB_id,
                       .data = {.SDO = {.Command = 0x40,
                                        .Index = index,
                                        .Sub_index = sub_index,
                                        .SDO_data = {.u32 = 0UL}}}};

  CAN_DEBUG_PRINT("0x%08X ", frame.data.SDO.Index);
  CAN_DEBUG_PRINT("\n");

  // write frame
  int status = ESP32Can.CANWriteFrame(&frame);
  if (status != 0) {
    CAN_DEBUG_PRINT("Error writing CAN frame!\n");
    return CAN_TX_ERROR;
  }
  return CAN_TX_SUCCESS;
}

int CAN_Manager::request_data(CAN_Data* data_to_request) {
  // check data is initialized
  if (!(data_to_request->initialized)) return CAN_DATA_NOT_INITIALIZED;
  uint8_t node_id = data_to_request->parent->node_id;
  uint16_t index = data_to_request->type.index;
  uint8_t sub_index = data_to_request->type.sub_index;
  // request
  return sdo_request_data(node_id, index, sub_index);
}

int CAN_Manager::sdo_write_data(CAN_Data* data_to_write) {
  CAN_DEBUG_PRINT("Writing data for node with id %d.\n", data_to_write->parent->node_id);
  CAN_DEBUG_PRINT("\t Requesting: ");

  // calculate canopen id
  uint32_t COB_id = 0x600L + data_to_write->parent->node_id;
  uint8_t command = 0x20;
  uint32_t data;
  switch(data_to_write->type.data_size){
    case 1: //1 byte
      command += 0xF;
      break;
    case 2: //2 bytes
      command += 0xB;
      break;
    case 4:
      command += 0x3;
      break;
    default:
      return CAN_TX_ERROR;
  }

  // construct frame. Command 0x40 - Read request
  CAN_frame_t frame = {.FIR = {.B =
                                   {
                                       .DLC = 8,
                                       .FF = CAN_frame_std,
                                   }},
                       .MsgID = COB_id,
                       .data = {.SDO = {.Command = command,
                                        .Index = data_to_write->type.index,
                                        .Sub_index = data_to_write->type.sub_index,
                                        .SDO_data = {.u32 = get<uint32_t>(data_to_write->data)}}}};

  CAN_DEBUG_PRINT("0x%08X ", frame.data.SDO.Index);
  CAN_DEBUG_PRINT("\n");

  // write frame
  int status = ESP32Can.CANWriteFrame(&frame);
  if (status != 0) {
    CAN_DEBUG_PRINT("Error writing CAN frame!\n");
    return CAN_TX_ERROR;
  }
  return CAN_TX_SUCCESS;
}

int CAN_Manager::request_next_segment(CAN_Data* data_to_request,
                                      bool toggle_bit) {
  // calculate canopen id
  uint32_t COB_id = 0x600L + data_to_request->parent->node_id;

  // block download + toggle bit
  uint8_t command = 0b11 << 5;
  if (toggle_bit) command += 0b1 << 4;

  // construct frame
  CAN_frame_t frame = {
      .FIR = {.B =
                  {
                      .DLC = 8,
                      .FF = CAN_frame_std,
                  }},
      .MsgID = COB_id,
      .data = {.SDO_domain = {.Command = command,
                              .SDO_data = {0, 0, 0, 0, 0, 0, 0}}}};

  int status = ESP32Can.CANWriteFrame(&frame);
  if (status != 0) {
    CAN_DEBUG_PRINT("Error writing CAN frame!\n");
    return CAN_TX_ERROR;
  }
  return CAN_TX_SUCCESS;
}

int CAN_Manager::send_nmt(CAN_Data* data_to_send) {
  if (data_to_send->type.message_type != MESSAGE_TYPE_NMT) return CAN_TX_ERROR;

  CAN_DEBUG_PRINT("Sending nmt command %d for node id %d.\n",
                  data_to_send->command, data_to_send->parent->node_id);

  // NMT id
  uint32_t COB_id = 0x000;
  // Construct frame
  CAN_frame_t frame = {
      .FIR = {.B =
                  {
                      .DLC = 8,
                      .FF = CAN_frame_std,
                  }},
      .MsgID = COB_id,
      .data = {.u8 = {data_to_send->command, data_to_send->parent->node_id, 0,
                      0, 0, 0, 0, 0}}};

  // write frame
  int status = ESP32Can.CANWriteFrame(&frame);
  if (status != 0) {
    CAN_ERROR_PRINT("Error writing CAN frame!\n");
    return CAN_TX_ERROR;
  }
  return CAN_TX_SUCCESS;
}

int CAN_Manager::send_pdo(CAN_Data* data_to_send) {
  if (data_to_send->type.message_type != MESSAGE_TYPE_RPDO) return CAN_TX_ERROR;

  if (!std::holds_alternative<Linak_RPDO>(data_to_send->data))
    return CAN_TX_ERROR;

  CAN_DEBUG_PRINT("Sending pdo to node id %d.\n",
                  data_to_send->parent->node_id);

  uint32_t COB_id =
      data_to_send->type.id_base_adress + data_to_send->parent->node_id;

  // Construct frame
  CAN_frame_t frame = {.FIR = {.B =
                                   {
                                       .DLC = 8,
                                       .FF = CAN_frame_std,
                                   }},
                       .MsgID = COB_id,
                       .data = {.rpdo = get<Linak_RPDO>(data_to_send->data)}};

  // write frame
  int status = ESP32Can.CANWriteFrame(&frame);
  if (status != 0) {
    CAN_ERROR_PRINT("Error writing CAN frame!\n");
    return CAN_TX_ERROR;
  }
  return CAN_TX_SUCCESS;
}

int CAN_Manager::send_vesc(CAN_Data* data_to_send) {
  if (data_to_send->type.message_type != MESSAGE_TYPE_VESC) {
    CAN_DEBUG_PRINT("not right datatype\n");
    return CAN_TX_ERROR;
  }

  if (!std::holds_alternative<int32_t>(data_to_send->data)){
    CAN_DEBUG_PRINT("Expected int32\n");
    return CAN_TX_ERROR;
  } 

  CAN_DEBUG_PRINT("Sending vesc to node id %d.\n",
                  data_to_send->parent->node_id);

  uint32_t COB_id = (((uint32_t)data_to_send->command) << 8) + (uint32_t)data_to_send->parent->node_id;

  CAN_DEBUG_PRINT("Command: %d\n", data_to_send->command)
  CAN_DEBUG_PRINT("COB ID: %08X\n", COB_id)
  // Construct frame
  CAN_frame_t frame = {.FIR = {.B =
                                   {
                                       .DLC = 8,
                                       .FF = CAN_frame_ext,
                                   }},
                       .MsgID = COB_id,
                       .data = {.u32 = {little_to_big_endian(get<int32_t>(data_to_send->data)), 0}}};

  // write frame
  int status = ESP32Can.CANWriteFrame(&frame);
  if (status != 0) {
    CAN_ERROR_PRINT("Error writing CAN frame!\n");
    return CAN_TX_ERROR;
  }
  return CAN_TX_SUCCESS;
}

int CAN_Manager::send_heartbeat(CAN_Data* data_to_send) {
  if (data_to_send->type.message_type != MESSAGE_TYPE_HEARTBEAT) return CAN_TX_ERROR;


  CAN_DEBUG_PRINT("Sending heartbeat messsage \n");

  uint32_t COB_id = 0x700 + 0x01;

  // Construct frame
  CAN_frame_t frame = {.FIR = {.B =
                                   {
                                       .DLC = 8,
                                       .FF = CAN_frame_std,
                                   }},
                       .MsgID = COB_id,
                       .data = {.u8 = {5, 0, 0, 0, 0, 0, 0, 0}}};

  // write frame
  int status = ESP32Can.CANWriteFrame(&frame);
  if (status != 0) {
    CAN_ERROR_PRINT("Error writing CAN frame!\n");
    return CAN_TX_ERROR;
  }
  return CAN_TX_SUCCESS;
}

/*===============================
          Receive functions
================================*/

void CAN_Manager::handle_vesc_receive(){
  unsigned long start_millis = millis();
  CAN_frame_t rx_frame;

  while ((start_millis + CAN_MAX_WAIT_TIME) > millis()) {

    if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 0) != pdTRUE) {
      if(rx_frame.MsgID > (0xFFFF) || rx_frame.MsgID < 0x1) continue; // if frame id data is larger than possible or zero


      //check against data in data vector
      uint8_t sent_from = rx_frame.MsgID & 0xFF; // vesc id is first byte
      uint8_t command_id = (rx_frame.MsgID & (0xFF << 8)) >> 8; // command is second byte

      //printf("Received vesc_status from %d (0x%02X) with command %d (0x%02X)\n",sent_from, sent_from, command_id, command_id);
      // printf("0x%04X\n", rx_frame.MsgID);

      for (const auto& data_ptr : Monitored_datas){

        if(data_ptr->parent->node_id == sent_from &&
            data_ptr->type.message_type == MESSAGE_TYPE_VESC_STATUS &&
            data_ptr->command == command_id &&
            std::holds_alternative<VESC_status_data>(data_ptr->data)){
              if(!xSemaphoreTake(data_ptr->parent->mutex, pdMS_TO_TICKS(10))) {
                CAN_ERROR_DEBUG("could not get mutex\n");
                return;
                }
              VESC_status_data received_data;
              switch(data_ptr->command){
                case VESC_STATUS_PACKET_1:
                  {int32_t erpm = little_to_big_endian(rx_frame.data.u32[0]);
                  int16_t current = little_to_big_endian(rx_frame.data.u16[2]);
                  int16_t duty = little_to_big_endian(rx_frame.data.u16[3]);

                  received_data = {.data = {.status1 = {
                                                                          .erpm = erpm,
                                                                          .current = current,
                                                                          .dutyCycle = duty
                  }}};
                  data_ptr->data = received_data;}
                break;
                case VESC_STATUS_PACKET_2:
                  {uint32_t Ah = little_to_big_endian(rx_frame.data.u32[0]);
                  uint32_t Ahc = little_to_big_endian(rx_frame.data.u32[1]);

                  received_data = {.data = {.status2 = {
                                                                          .ampHours = Ah,
                                                                          .ampHoursChg = Ahc
                  }}};
                  data_ptr->data = received_data;}
                break;
                case VESC_STATUS_PACKET_3:
                  {uint32_t Wh = little_to_big_endian(rx_frame.data.u32[0]);
                  uint32_t Whc = little_to_big_endian(rx_frame.data.u32[1]);

                  received_data = {.data = {.status3 = {
                                                                          .wattHours = Wh,
                                                                          .wattHoursChg = Whc
                  }}};
                  data_ptr->data = received_data;}
                break;
                case VESC_STATUS_PACKET_4:
                  {int16_t tempFet = little_to_big_endian(rx_frame.data.u16[0]);
                  int16_t tempMot = little_to_big_endian(rx_frame.data.u16[1]);
                  int16_t currentIn = little_to_big_endian(rx_frame.data.u16[2]);
                  int16_t pid = little_to_big_endian(rx_frame.data.u16[3]);

                  received_data = {.data = {.status4 = {
                                                                          .tempFET = tempFet,
                                                                          .tempMotor = tempMot,
                                                                          .currentIn = currentIn,
                                                                          .pidPos = pid
                  }}};
                  data_ptr->data = received_data;}
                break;
                case VESC_STATUS_PACKET_5:
                  {uint32_t tachometer = little_to_big_endian(rx_frame.data.u32[0]);
                  int16_t vin = little_to_big_endian(rx_frame.data.u16[3]);

                  received_data = {.data = {.status5 = {
                                                                          .tachometer = tachometer,
                                                                          .voltsIn = vin
                  }}};
                  data_ptr->data = received_data;}
                break;
                case VESC_STATUS_PACKET_6:
                  {int16_t adc1 = little_to_big_endian(rx_frame.data.u16[0]);
                  int16_t adc2 = little_to_big_endian(rx_frame.data.u16[1]);
                  int16_t adc3 = little_to_big_endian(rx_frame.data.u16[2]);
                  int16_t ppm = little_to_big_endian(rx_frame.data.u16[3]);

                  received_data = {.data = {.status6 = {
                                                                          .adc1 = adc1,
                                                                          .adc2 = adc2,
                                                                          .adc3 = adc3,
                                                                          .ppm = ppm
                  }}};
                  data_ptr->data = received_data;}
                break;
              }
              data_ptr->updated = true;
              xSemaphoreGive(data_ptr->parent->mutex);
              xEventGroupSetBits(CAN_status_flags, CAN_FLAG_NEW_MONITORED_AVAILABLE);
              return;
            }
      }    
    }
  }
}


int CAN_Manager::receive_request_response(uint8_t node_id, uint16_t index,
                                          uint8_t sub_index,
                                          CAN_frame_t* rx_frame) {
  unsigned long start_millis = millis();

  // while loop required due to neccesity to receive potentially many packets
  // before the expected one arrives could potentially be mitigated by using
  // filters?
  while ((start_millis + CAN_MAX_WAIT_TIME) > millis()) {
    if (xQueueReceive(CAN_cfg.rx_queue, rx_frame, 0) != pdTRUE) {
      CAN_DEBUG_PRINT("Received frame from 0x%08X with index 0x%08X\n",
                      rx_frame->MsgID - 0x580, rx_frame->data.SDO.Index);
      // if frame matching request received
      if (rx_frame->data.SDO.Index == index &&
          rx_frame->data.SDO.Sub_index == sub_index &&
          (rx_frame->MsgID - 0x580) == node_id) {
        CAN_DEBUG_PRINT("\tRecived frame matching request!\n");
        return CAN_RX_SUCCESS;
      }
    }
    vTaskDelay(2 * portTICK_PERIOD_MS);  // wait before testing again
  }
  CAN_DEBUG_PRINT("Receive of CAN frame timed out!\n");
  return CAN_RX_ERROR;
}

int CAN_Manager::receive_request_response(CAN_Data* data_to_receive,
                                          CAN_frame_t* rx_frame) {
  return receive_request_response(data_to_receive->parent->node_id,
                                  data_to_receive->type.index,
                                  data_to_receive->type.sub_index, rx_frame);
}

int CAN_Manager::receive_string_segment(CAN_Data* data_to_receive,
                                        bool toggle_bit, String& data) {
  unsigned long start_millis = millis();
  CAN_frame_t rx_frame;

  // while loop required due to neccesity to receive potentially many packets
  // before the expected one arrives could potentially be mitigated by using
  // filters?
  while ((start_millis + CAN_MAX_WAIT_TIME) > millis()) {
    if (xQueueReceive(CAN_cfg.rx_queue, &rx_frame, 0) != pdTRUE) {
      // if frame matching request received
      if ((rx_frame.MsgID - 0x580) == data_to_receive->parent->node_id &&
          (rx_frame.data.u8[0] & 0b1 << 4) ==
              1 * toggle_bit << 4) {  // check matching toggle bit

        int amount_bytes_not_used = (rx_frame.data.u8[0] & 0b111 << 1) >> 1;
        CAN_DEBUG_PRINT("Recived frame matching request! (not used:%d)\n",
                        amount_bytes_not_used);

        // append data to storage String
        for (int i = 1; i < 8 - amount_bytes_not_used; i++) {
          data += (char)rx_frame.data.u8[i];
        }

        // if c-bit is set, return end of segment
        if (rx_frame.data.u8[0] & 0b1)
          return CAN_RX_END_OF_SEGMENT;
        else
          return CAN_RX_SUCCESS;
      }
    }
    vTaskDelay(2 * portTICK_PERIOD_MS);  // wait before testing again
  }
  // if timeout
  CAN_ERROR_PRINT("Receive of CAN frame timed out!\n");
  return CAN_RX_ERROR;
}

/*===============================
          Store functions
================================*/

void CAN_Manager::write_value(CAN_Data* data_to_write,
                              CAN_frame_t* update_frame) {
  CAN_DEBUG_PRINT("Attemting to write data to 0x%04X\n",
                  data_to_write->type.index);

  // take frame data and extract and cast to appripriate type. Then store it.
  if (data_to_write->type.data_size == sizeof(uint8_t)) {
    if (data_to_write->type.sign) {
      int8_t new_data =
          static_cast<int8_t>(update_frame->data.SDO.SDO_data.u8[0]);
      data_to_write->data = static_cast<int32_t>(new_data);

    } else {
      uint8_t new_data = update_frame->data.SDO.SDO_data.u8[0];
      data_to_write->data = static_cast<uint32_t>(new_data);
    }
  } else if (data_to_write->type.data_size == sizeof(uint16_t)) {
    if (data_to_write->type.sign) {
      int16_t new_data =
          static_cast<int16_t>(update_frame->data.SDO.SDO_data.u8[0]) |
          (static_cast<int16_t>(update_frame->data.SDO.SDO_data.u8[1]) << 8);
      data_to_write->data = static_cast<int32_t>(new_data);
    } else {
      uint16_t new_data =
          static_cast<uint16_t>(update_frame->data.SDO.SDO_data.u8[0]) |
          (static_cast<uint16_t>(update_frame->data.SDO.SDO_data.u8[1]) << 8);
      data_to_write->data = static_cast<uint32_t>(new_data);
    }
  } else if (data_to_write->type.data_size == sizeof(uint32_t)) {
    if (data_to_write->type.sign) {
      int32_t new_data =
          static_cast<int32_t>(update_frame->data.SDO.SDO_data.u8[0]) |
          (static_cast<int32_t>(update_frame->data.SDO.SDO_data.u8[1]) << 8) |
          (static_cast<int32_t>(update_frame->data.SDO.SDO_data.u8[2]) << 16) |
          (static_cast<int32_t>(update_frame->data.SDO.SDO_data.u8[3]) << 24);
      data_to_write->data = static_cast<int32_t>(new_data);
    } else {
      uint32_t new_data =
          static_cast<uint32_t>(update_frame->data.SDO.SDO_data.u8[0]) |
          (static_cast<uint32_t>(update_frame->data.SDO.SDO_data.u8[1]) << 8) |
          (static_cast<uint32_t>(update_frame->data.SDO.SDO_data.u8[2]) << 16) |
          (static_cast<uint32_t>(update_frame->data.SDO.SDO_data.u8[3]) << 24);
      data_to_write->data = static_cast<uint32_t>(new_data);
    }
  }
}

/*===============================
             Tools
================================*/

void CAN_Manager::update_data(CAN_Data* data_to_update) {
  if (!(data_to_update->initialized)) {
    CAN_ERROR_PRINT("Cannot update data: Data not initialized!\n");
    return;
  }
  // select appropriate function depending on datatype
  if (data_to_update->type.datatype == DATATYPE_NUMBER) {
    update_number(data_to_update);
  } else if (data_to_update->type.datatype == DATATYPE_STRING) {
    update_string(data_to_update);
  } else if (data_to_update->type.datatype == DATATYPE_BITFEILD) {
    update_bitfield(data_to_update);
  }
}

int CAN_Manager::scan_for_device(uint8_t id_to_scan) {
  // request device-id index 0x1000 subindex 0
  if (sdo_request_data(id_to_scan, 0x1000, 0) != CAN_TX_SUCCESS) return -1;

  // try to receive, if device responds it is considered found!
  CAN_frame_t rx_frame;
  return (receive_request_response(id_to_scan, 0x1000, 0, &rx_frame) ==
          CAN_RX_SUCCESS);
}

void CAN_Manager::check_can_errors() {
  // error registers
  unsigned int err_st = MODULE_CAN->SR.B.ES;
  unsigned int bus_off = MODULE_CAN->SR.B.BS;

  // printf("Error bits status err:%d errOld:%d bus:%d busOld:%d\n", err_st,
  // err_st_old, bus_off, bus_off_old);
  if ((err_st != err_st_old) || (bus_off != bus_off_old)) {
    if ((err_st == 0) && (bus_off == 0)) {  // no error
      CAN_ERROR_PRINT("CAN error resolved.\n");
      xEventGroupClearBits(CAN_status_flags,
                           CAN_FLAG_CAN_ERROR);  // error no longer active
    } else if ((err_st == 1) &&
               (bus_off == 0)) {  // TX or RX errors exceeding limit
      CAN_ERROR_PRINT(
          "TX or RX errors exceeding limit. Restarting CAN module.\n");
      xEventGroupSetBits(CAN_status_flags, CAN_FLAG_CAN_ERROR);
      xEventGroupClearBits(CAN_status_flags,
                           CAN_FLAG_TX_QUEUE_EMPTY);  // if can error active,
                                                      // set queue to not empty
      restart();                                   // init recovery
    } else if ((err_st == 1) && (bus_off == 1)) {  // CAN bus entered off state
      CAN_ERROR_PRINT("CAN bus entered off state. Restarting CAN module.\n");
      xEventGroupSetBits(CAN_status_flags, CAN_FLAG_CAN_ERROR);
      xEventGroupClearBits(CAN_status_flags,
                           CAN_FLAG_TX_QUEUE_EMPTY);  // if can error active,
                                                      // set queue to not empty
      restart();
      // note, not sure if this will exit bus of state. But there is always the
      // reset button :)

    } else if ((err_st == 0) && (bus_off == 1)) {  // Bus error limit recovered
      CAN_ERROR_PRINT("Bus error limit recovered.\n");
      xEventGroupSetBits(CAN_status_flags, CAN_FLAG_CAN_ERROR);
      xEventGroupClearBits(CAN_status_flags,
                           CAN_FLAG_TX_QUEUE_EMPTY);  // if can error active,
                                                      // set queue to not empty
    }
    // remember states do detect changes next time
    err_st_old = err_st;
    bus_off_old = bus_off;
  } else if ((err_st == 0) && (bus_off == 0)) {
    xEventGroupClearBits(CAN_status_flags,
                         CAN_FLAG_CAN_ERROR);  // no error
  }
}

void CAN_Manager::restart() {
  unsigned int err_st = MODULE_CAN->SR.B.ES;
  while (err_st != 0) {  // while error active
    while (sdo_request_data(0, 0x1000, 0) !=
           CAN_TX_SUCCESS) {  // send message to decrement error counter
      // maybe below code recovers from bus of, but appears to cause problems
      //  ESP32Can.CANStop();
      //  ESP32Can.CANInit();
      vTaskDelay(pdMS_TO_TICKS(50));
    }
    // when it is possible to send messages, wait for error counters to
    // decrement to 0
    unsigned int rec = MODULE_CAN->RXERR.B.RXERR;  // receive error counter
    unsigned int tec = MODULE_CAN->TXERR.B.TXERR;  // transmit error counter
    CAN_ERROR_PRINT("rec: %d, tec: %d\n", rec, tec)
    vTaskDelay(pdMS_TO_TICKS(5));  // dont spam, and allow other tasks to exist
    err_st = MODULE_CAN->SR.B.ES;
  }
}

void CAN_Manager::loop_wrapper(void* pvParameters) {
  // this maybe allows for multiple can_manager objects to run. Maybe is
  // reddundant.
  CAN_Manager* canManager = static_cast<CAN_Manager*>(pvParameters);
  canManager->loop();
}



void CAN_Manager::loop() {
  for (;;) {
    EventBits_t current_function_flags = xEventGroupGetBits(CAN_function_flags);

    // If no active error
    if ((xEventGroupGetBits(CAN_status_flags) & CAN_FLAG_CAN_ERROR) == 0) {
      // data requests
      CAN_Data* message_to_send;
      if (xQueueReceive(message_send_queue, &message_to_send, 0) == pdTRUE) {
        xEventGroupClearBits(CAN_status_flags, CAN_FLAG_TX_QUEUE_EMPTY);

        uint8_t parent_id = message_to_send->parent->node_id;
        CAN_DEBUG_PRINT("Received from queue\n");
        CAN_DEBUG_PRINT(" %s parent: %d \n", message_to_send->type.name.c_str(),
                        parent_id);

        xSemaphoreTake(message_to_send->parent->mutex,
                       portMAX_DELAY);  // dont let anyone else modify data in
                                        // device during update
        CAN_DEBUG_PRINT("Unqueue data, id:%d, index:0x%04X\n", parent_id,
                        message_to_send->type.index);

        switch(message_to_send->type.message_type){
          case MESSAGE_TYPE_SDO_READ:
            CAN_DEBUG_PRINT("SDOR Message\n");
            update_data(message_to_send);
          break;
          case MESSAGE_TYPE_SDO_WRITE:
            CAN_DEBUG_PRINT("SDOW Message\n");
            sdo_write_data(message_to_send);
          break;
          case MESSAGE_TYPE_RPDO:
            CAN_DEBUG_PRINT("RPDO Message\n");
            send_pdo(message_to_send);
          break;
          case MESSAGE_TYPE_VESC:
            CAN_DEBUG_PRINT("VESC Message\n");
            send_vesc(message_to_send);
          break;
          case MESSAGE_TYPE_HEARTBEAT:
            CAN_DEBUG_PRINT("HEARTBEAT Message\n");
            send_heartbeat(message_to_send);
          break;
          case MESSAGE_TYPE_NMT:
            CAN_DEBUG_PRINT("NMT Message\n");
            send_nmt(message_to_send);
            break;
          default:
          break;
        }
        xSemaphoreGive(message_to_send->parent->mutex);

        handle_vesc_receive(); // receive vesc status
      }

      // if no tasks waiting
      if (uxQueueMessagesWaiting(message_send_queue) == 0)
        xEventGroupSetBits(CAN_status_flags, CAN_FLAG_TX_QUEUE_EMPTY);

      // if indicated empty but not empty, set not empty
      else if ((xEventGroupGetBits(CAN_status_flags) &
                CAN_FLAG_TX_QUEUE_EMPTY) != 0)
        xEventGroupClearBits(CAN_status_flags, CAN_FLAG_TX_QUEUE_EMPTY);
    } else {  // can error active!
      xEventGroupClearBits(CAN_status_flags,
                           CAN_FLAG_TX_QUEUE_EMPTY);  // if can error active,
                                                      // set queue to not empty
    }

    check_can_errors();
    // check queues every 5 ms
    vTaskDelay(pdMS_TO_TICKS(1));
  }
}

/*======================================
          Public functions
========================================*/

void CAN_Manager::init() {
  // configure CAN setings
  pinMode(CAN_SE_PIN, OUTPUT);
  digitalWrite(CAN_SE_PIN, LOW);
  CAN_cfg.speed = CAN_SPEED_500KBPS;
  CAN_cfg.tx_pin_id = GPIO_NUM_27;
  CAN_cfg.rx_pin_id = GPIO_NUM_26;

  // Initialize RTOS objects
  while (1) {  // do not exit this loop until all RTOS objects are initialized
    CAN_cfg.rx_queue = xQueueCreate(rx_queue_size, sizeof(CAN_frame_t));
    CAN_cfg.tx_queue = xQueueCreate(tx_queue_size, sizeof(CAN_frame_t));

    message_send_queue = xQueueCreate(tx_queue_size, sizeof(CAN_Data*));
    CAN_function_flags = xEventGroupCreate();
    CAN_status_flags = xEventGroupCreate();

    // check if all are initialized correctly and break out of loop.
    if (message_send_queue != nullptr && CAN_function_flags != nullptr && CAN_cfg.rx_queue != nullptr && CAN_cfg.tx_queue != nullptr) break;
    CAN_DEBUG_PRINT("Could not initiate all rtos components, retrying.\n");
  }

  // init error state vars
  err_st_old = 0;
  bus_off_old = 0;

  // Init CAN Module
  ESP32Can.CANInit();

  // start CAN_loop
  xTaskCreatePinnedToCore(&CAN_Manager::loop_wrapper, "CAN_loop", 4096, this, 1,
                          &CAN_task, 1);
  CAN_DEBUG_PRINT("CAN started\n");
}

