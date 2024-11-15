#ifndef CAN_OPEN_H
#define CAN_OPEN_H

#include <Arduino.h>
#include <queue>
#include <typeinfo>
#include <variant>
#include <vector>
#include <memory>

#include "../lib/ESP32-Arduino-CAN/ESP32CAN.h"
#include "../lib/ESP32-Arduino-CAN/can_regdef.h"

#include "../datalayer/datalayer.h"
#include "../config.h"
#include "../serial_comms/debug.h"

// #include "../include.h"



#ifdef CAN_DEBUG 
#define CAN_DEBUG_PRINT(format, ...) debug_message("CAN: Debug: ", format, ##__VA_ARGS__);
#else
#define CAN_DEBUG_PRINT(...)
#endif

#ifdef CAN_ERROR_DEBUG
#define CAN_ERROR_PRINT(format, ...) debug_message("CAN: Error: ", format, ##__VA_ARGS__);
#else
#define CAN_ERROR_PRINT(...)
#endif


#define CAN_MAX_NODE_ID 32 //this can be increased to max 64. After that the register is too small


//Status flags
#define CAN_FLAG_TX_QUEUE_EMPTY (1 << 0)
#define CAN_FLAG_NEW_MONITORED_AVAILABLE (1<<1)
#define CAN_FLAG_CAN_ERROR (1<<7)



//Funtion return codes
#define CAN_TX_ERROR 0
#define CAN_RX_ERROR 1
#define CAN_TX_SUCCESS 2
#define CAN_RX_SUCCESS 3
#define CAN_RX_END_OF_SEGMENT 4
#define CAN_DATA_NOT_INITIALIZED 5


//NMT commands
#define NMT_START 1
#define NMT_STOP 2
#define NMT_PRE_OPERATIONAL 128
#define NMT_RESET 129
#define NMT_RESET_COMM 130
#define NMT_ENTER_SLEEP 80
#define NMT_ENTER_STANDBY 96

// VESC STUFF
// Command types
#define VESC_COMMAND_SET_DUTY                0U
#define VESC_COMMAND_SET_CURRENT             1U
#define VESC_COMMAND_SET_CURRENT_BRAKE       2U
#define VESC_COMMAND_SET_RPM                 3U
#define VESC_COMMAND_SET_POS                 4U
#define VESC_COMMAND_SET_PID_RPM             5U
#define VESC_COMMAND_SET_CURRENT_REL         10U
#define VESC_COMMAND_SET_CURRENT_BRAKE_REL   11U
#define VESC_COMMAND_SET_CURRENT_HANDBRAKE   12U
#define VESC_COMMAND_SET_CURRENT_HANDBRAKE_REL 13U

// Status command types
#define VESC_STATUS_PACKET_1       9
#define VESC_STATUS_PACKET_2      14
#define VESC_STATUS_PACKET_3      15
#define VESC_STATUS_PACKET_4      16
#define VESC_STATUS_PACKET_5      27
#define VESC_STATUS_PACKET_6      28


class CAN_Manager {
 private:

  //keeps track of old error state
  unsigned int err_st_old;
  //keeps track of old bus-off state
  unsigned int bus_off_old;
 
  

 
  /*===============================
          Update helpers
  ================================*/

  /**
   * Helper function to update NUMBER-type data objects. Sends request via CAN, receives data and stores it.
   * @param data_to_update raw pointer to CAN_Data object to update. Must be correctly initialized and of type DATATYPE_NUMBER.
   */
  void update_number(CAN_Data* data_to_update);

  /**
   * Helper function to update STRING-type data objects. Sends request via CAN, receives data and stores it.
   * @param data_to_update raw pointer to CAN_Data object to update. Must be correctly initialized and of type DATATYPE_STRING.
   */
  void update_string(CAN_Data* data_to_update);

  /**
   * Helper function to update BITFIELD-type data objects. Sends request via CAN, receives data and stores it.
   * @param data_to_update raw pointer to CAN_Data object to update. Must be correctly initialized and of type DATATYPE_BITFIELD.
   */
  void update_bitfield(CAN_Data* data_to_update);

  
  /*===============================
      Transmit functions
  ================================*/
  /**
   * General function to request SDO-info. Sends request frame with command 0x40.
   * @param node_id id of CAN device to request SDO from
   * @param index SDO index to request
   * @param sub_index SDO subindex to request
   * @return CAN_TX_SUCCESS if tx was succesful. 
   *         CAN_XX_ERROR if timeout or other error occured.
   */
  int sdo_request_data(uint8_t node_id, uint16_t index, uint8_t sub_index);

  /**
   * This will send a SDO request with parameters corresonding to a CAN_Data object.
   * Wrapper for request_data(...).
   * @param data_to_request pointer to the CAN_Data object.
   * @return CAN_TX_SUCCESS if tx was succesful. 
   *         CAN_XX_ERROR if timeout or other error occured.
   */
  int request_data(CAN_Data* data_to_request);


  int sdo_write_data(CAN_Data* data_to_write);


  /**
   * For segmented transfers (eg. Strings). Constructs and sends SDO command for requesting next segment for data contained in CAN_Data object.
   * @param data_to_request pointer to the CAN_Data object.
   * @param toggle_bit CAN toggle bit, alternating between segments.
   * @return CAN_TX_SUCCESS if tx was succesful. 
   *         CAN_XX_ERROR if timeout or other error occured.
   */
  int request_next_segment(CAN_Data* data_to_request, bool toggle_bit);


  /**
   * Transmits an NMT command
   * @param request pointer to CAN_Data containing request information.
   * @return CAN_TX_SUCCESS if tx was succesful. 
   *         CAN_XX_ERROR if timeout or other error occured.
   */
  int send_nmt(CAN_Data* data_to_send);

  int send_pdo(CAN_Data* data_to_send);

  int send_vesc(CAN_Data* data_to_send);

  int send_heartbeat(CAN_Data* data_to_send);

  /*===============================
          Receive functions
  ================================*/


  void handle_vesc_receive();

  /**
   * Waits until SDO response is received. If succesful, saves the received frame.
   * @param node_id id of CAN device to receive SDO from
   * @param index SDO index to receive
   * @param sub_index SDO subindex to receive
   * @param rx_frame pointer to CAN_frame_t object to store received frame in, if succesful.
   * @return CAN_RX_SUCCESS if recive was succesful and frame stored. 
   *         CAN_RX_ERROR if timeout or receive was unsuccesful.
   */
  int receive_request_response(uint8_t node_id, uint16_t index, uint8_t sub_index,
                                  CAN_frame_t* rx_frame);

  /**
   * Waits until SDO response with data corresponding with CAN_Data object is received. If succesful, saves the received frame. Wrapper for receive_request_response(...).
   * @param data_to_receive pointer to CAN_Data object
   * @param rx_frame pointer to CAN_frame_t object to store received frame in, if succesful.
   * @return CAN_RX_SUCCESS if recive was succesful and frame stored.
   *         CAN_RX_ERROR if timeout or receive was unsuccesful.
   */
  int receive_request_response(CAN_Data* data_to_receive,
                               CAN_frame_t* rx_frame);

  
  /**
   * Function for handeling segmented string transfers.
   * Waits for a segment to be received and appends the information contained onto an existng string.
   * @param data_to_receive pointer to data object with the expected data
   * @param toggle_bit expected toggle bit, alternating between segments
   * @param data String to append segment to.
   * @return CAN_RX_SUCCESS if recive was succesful and segment stored.
   *         CAN_RX_END_OF_SEGMENT if recive was succesful, segment stored and end of transfer was received.
   *         CAN_RX_ERROR if timeout or receive was unsuccesful.
   */
  int receive_string_segment(CAN_Data* data_to_receive, bool toggle_bit,
                             String& data);

  
  /*===============================
          Store functions
  ================================*/
  
  /**
   * Saves the data transmitted in a CAN frame into a CAN_Data object
   * @param data_to_write pointer to the data object in wich the transmitted data should be saved
   * @param update_frame pointer to the received frame containing updated information
   */
  void write_value(CAN_Data* data_to_write, CAN_frame_t* update_frame);
  


  /*===============================
             Tools
  ================================*/


   /**
   * Executes update of a data object. Sends request via CAN, receives data and stores it.
   * @param data_to_update raw pointer to CAN_Data object to update. Must be correctly initialized.
   */
  void update_data(CAN_Data* data_to_update);
  
  /**
   * Scans for a device
   * @param id_to_scan can id to scan for
   * @return 1 if device found, 0 not found, -1 if can error occured
   */
  int scan_for_device(uint8_t id_to_scan);

  /**
   * Checks for CAN errors and raises error flags if found.
   */
  void check_can_errors();

  /**
   * Restarts the can interface, recovers from tx errors.
   */
  void restart();

    /**
   * Main loop of CAN driver.
   * THIS IS NOT TO BE RUN.
   */
  void loop();

  /**
   * Wrapper function for main loop, allows for it to be run as a thread.
   * THIS IS RUN AS A TASK
   */
  static void loop_wrapper(void* pvParameters);

 public:
 /**
  * Initializes and starts the CAN driver. The driver itself is running as a separate thread started by this function.
  * No additional function calls are required to start CAN.
  */
  void init();

  //handle to CAN loop task
  TaskHandle_t CAN_task;

  /**
   * Queue for data update requests. Holds pointers to CAN_Data objects.
   * To enqueue/submit data update requests, follow this rough example:
   *  CAN_Data* pointer = pointer_to_object;
   *  xQueueSendToBack(data_update_queue, &pointer, ...)
   */ 
  xQueueHandle message_send_queue;



  /**
   * Register for reading what devices were discovered after a scan. 
   * bit 0 indicates device index 0 and so on.
   * If the bit is set that means a device was found.
   */
  uint64_t device_discovery_register;

  /**
   * CAN flags
   * bit 0 - request device scan, set to start
   * bit 1 - device scan complete, register can be read, gets set when done
   * bit 7 - 
   */
  EventGroupHandle_t CAN_function_flags;

  /**
   * CAN status flags, indicates current status of the CAN driver
   * bit 0 - data_update_queue empty
   * bit 1 - New Monitored data available
   * bit 7 - CAN error
   */
  EventGroupHandle_t CAN_status_flags;



  /**
   * Vector containing CAN_Data objects that are to be monitored and updated continiously.
   */
  std::vector<std::shared_ptr<CAN_Data>> Monitored_datas;
  
};

extern CAN_Manager CAN;
#endif