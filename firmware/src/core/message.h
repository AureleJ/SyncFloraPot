#ifndef MESSAGES_H
#define MESSAGES_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum {
    CMD_WATER_DURATION,
    CMD_WATER_MOISTURE,
    CMD_WATER_STOP
} hydration_cmd_t;

typedef struct {
    hydration_cmd_t command;
    uint32_t duration_ms; 
    uint8_t target_moisture;
} hydration_msg_t;

extern QueueHandle_t hydration_queue;

#endif // MESSAGES_H