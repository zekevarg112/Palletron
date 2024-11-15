#ifndef SERIAL_PARSER_H
#define SERIAL_PARSER_H
#include "../include.h"

#ifdef SERIAL_PARSER_DEBUG 
#define SERIAL_PARSER_DEBUG_PRINT(format, ...) debug_message("Serial parser: Debug: ", format, ##__VA_ARGS__);
#else
#define SERIAL_PARSER_DEBUG_PRINT(format, ...)
#endif

#define SERIAL_COMMAND_RPM 3U
#define SERIAL_COMMAND_ENCODER 40U


typedef struct{
    bool succes; // set to true if parse succeeded
    uint8_t device; // 1-right, 2-left, 3-both
    uint8_t command;
    float data;
} serial_parsed_data;


typedef struct __attribute__((packed)){
    union{
    struct {
        uint8_t start_byte;
        uint8_t command;
        uint8_t u8_float_data[4];
        uint8_t checksum;
    } data;
    uint8_t u8[7];
    };
} serial_rx_data;

/***
Checks and parses serial vesc commands. They should be in the following format.
7 bytes:
    @param byte1  uint8_t start_byte: xpected 0xA5
    @param byte2  uint8_t command: what device and what command. Formatted as: command + device<<6. Where command = 0-63 and device is 1-right, 2-left, 3-both.
    @param bytes3to6  float data: 4-byte float in little endian format
    @param byte7  uint8_t checksum: checksum

Returns serial_parsed_data object
***/
serial_parsed_data check_and_parse_serial();

void send_vesc_rpm(CAN_Device* device, bool right);

void send_encoder_angle(float angle);



#endif