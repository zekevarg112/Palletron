#ifndef DEBUG_H
#define DEBUG_H
#include <Arduino.h>
#include <cstdarg>
#include <cstring>
#include "../config.h"

void debug_message(const char* prefix, const char* format, ...);


void debug_init();

extern xSemaphoreHandle serial_mutex;

#endif




