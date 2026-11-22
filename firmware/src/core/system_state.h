#ifndef SYSTEM_STATE_H
#define SYSTEM_STATE_H

#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"

typedef enum
{
    SYS_STATE_BOOT,
    SYS_STATE_PROVISIONING,
    SYS_STATE_CONNECTING,
    SYS_STATE_CONNECTED,
    SYS_STATE_ERROR
} system_state_t;

#define SYS_EVT_WIFI_CONNECTED (1 << 0)
#define SYS_EVT_CLOUD_CONNECTED (1 << 1)
#define SYS_EVT_SENSOR_ALERT (1 << 2)

void system_state_init(void);

void system_state_set(system_state_t state);

system_state_t system_state_get();

EventGroupHandle_t system_state_get_event_group();

#endif // SYSTEM_STATE_H