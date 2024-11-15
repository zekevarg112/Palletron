#ifndef DATALAYER_H
#define DATALAYER_H

#include <Arduino.h>
#include <stdio.h>
#include <vector>
#include <variant>
#include <map>
#include <memory>
#include <variant>
#include "../lib/ESP32-Arduino-CAN/CAN.h"

typedef struct CAN_Device CAN_Device;

#define DATATYPE_STRING 0x1
#define DATATYPE_NUMBER 0x2
#define DATATYPE_BITFEILD 0x3

#define MESSAGE_TYPE_SDO_READ 0x1
#define MESSAGE_TYPE_SDO_WRITE 0x2
#define MESSAGE_TYPE_TPDO 0x3
#define MESSAGE_TYPE_RPDO 0x4
#define MESSAGE_TYPE_NMT 0x5
#define MESSAGE_TYPE_HEARTBEAT 0x6
#define MESSAGE_TYPE_VESC 0x7
#define MESSAGE_TYPE_VESC_STATUS 0x8


typedef struct {
    int32_t erpm;        // B0 - B3, scale 1
    int16_t current;      // B4 - B5, scale 10
    int16_t dutyCycle;    // B6 - B7, scale 1000
} VESC_status_1;

typedef struct {
    uint32_t ampHours;      // B0 - B3, scale 10000
    uint32_t ampHoursChg;   // B4 - B7, scale 10000
} VESC_status_2;

typedef struct {
    uint32_t wattHours;      // B0 - B3, scale 10000
    uint32_t wattHoursChg;   // B4 - B7, scale 10000
} VESC_status_3;

typedef struct {
    int16_t tempFET;     // B0 - B1, scale 10
    int16_t tempMotor;   // B2 - B3, scale 10
    int16_t currentIn;   // B4 - B5, scale 10
    int16_t pidPos;      // B6 - B7, scale 50
} VESC_status_4;

typedef struct {
    uint32_t tachometer;  // B0 - B3, scale 6
    int16_t voltsIn;      // B4 - B5, scale 10
} VESC_status_5;

typedef struct {
    int16_t adc1;   // B0 - B1, scale 1000
    int16_t adc2;   // B2 - B3, scale 1000
    int16_t adc3;   // B4 - B5, scale 1000
    int16_t ppm;    // B6 - B7, scale 1000
} VESC_status_6;

typedef struct {
    union {
        VESC_status_1 status1;
        VESC_status_2 status2;
        VESC_status_3 status3;
        VESC_status_4 status4;
        VESC_status_5 status5;
        VESC_status_6 status6;
    } data;
} VESC_status_data;






/**
 * Struct defining datatype:
 * type: 1(String), 2(Number), 3(Bitfeild)\n
 * name: required
 * subname: required
 * index: required
 * sub_index: required
 * size: only reqired if type is 2(number) or 3(bitfeild)
 * scale: only reqired if type is 2(number)
 * sign: only reqired if type is 2(number)
 */
typedef struct {
  uint8_t message_type;
  uint16_t id_base_adress; //0x200 for TPDO, 0x100 for RPDO
  uint8_t datatype;
  uint8_t data_size;
  String name;  // change to static length variable?
  uint16_t index; //only SDO
  uint8_t sub_index; //only SDO
  int8_t scale;
  bool sign;
} CAN_Datatype;


typedef struct {
  CAN_Datatype type;
  bool initialized;
  CAN_Device* parent;
  std::variant<String, uint32_t, int32_t, Linak_TPDO, Linak_RPDO, VESC_status_data> data; //variant can hold different datatypes under same variable name
  uint8_t command; //used for nmt
  std::map<uint8_t, String> bitfeild_names; //only used if datatype is bitfield
  bool updated;
} CAN_Data;


struct CAN_Device{
  bool active;
  SemaphoreHandle_t mutex;
  uint8_t node_id;
  String friendly_name;
  std::shared_ptr<CAN_Data> data;
};

class Datalayer{
    public:
    // std::map<uint8_t, std::shared_ptr<CAN_Device>> devices;
    CAN_Device Motor_R;
    CAN_Device Motor_L;
};

extern Datalayer datalayer;

#endif