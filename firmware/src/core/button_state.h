#ifndef BUTTON_STATE_H
#define BUTTON_STATE_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

typedef enum {
    BTN_EVENT_NONE,
    BTN_EVENT_SINGLE_CLICK,
    BTN_EVENT_LONG_PRESS, 
} button_event_t;

extern QueueHandle_t button_event_queue;

#endif // BUTTON_STATE_H