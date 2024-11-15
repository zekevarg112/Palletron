#ifndef ESP32CAN_H
#define ESP32CAN_H

#include "CAN_config.h"
#include "CAN.h"
#include "../../config.h"
#include <Arduino.h>
#include "../../serial_comms/debug.h"

class ESP32CAN
{
    public: 
        bool tx_ok = true;
        int CANInit();
		int CANConfigFilter(const CAN_filter_t* p_filter);
        int CANWriteFrame(const CAN_frame_t* p_frame);
        int CANStop();
};

extern ESP32CAN ESP32Can;
#endif
