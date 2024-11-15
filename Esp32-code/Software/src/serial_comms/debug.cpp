#include "debug.h"

void debug_message(const char* prefix, const char* format, ...) {
    xSemaphoreTake(serial_mutex, portMAX_DELAY);
    char combined_format[256];  // Buffer for the combined format string

    // Combine the prefix and the provided format string
    snprintf(combined_format, sizeof(combined_format), "%s%s", prefix, format);

    // Handle the variable argument list
    va_list args;
    va_start(args, format);
    vprintf(combined_format, args);  // Use the combined format string
    va_end(args);
    xSemaphoreGive(serial_mutex);
}

void debug_init(){
    Serial.begin(115200);
    serial_mutex = xSemaphoreCreateBinary();
    xSemaphoreGive(serial_mutex);
}