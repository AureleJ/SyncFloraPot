#ifndef SRV_BUTTON_H
#define SRV_BUTTON_H

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"

void srv_button_init(gpio_num_t button_pin);

#endif // SRV_BUTTON_H