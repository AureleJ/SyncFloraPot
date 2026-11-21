#ifndef SRV_BUTTON_H
#define SRV_BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void srv_button_init(void);

#endif // SRV_BUTTON_H