#ifndef BUTTON_H
#define BUTTON_H

#include "driver/gpio.h"
#include "esp_err.h"

esp_err_t button_init(gpio_num_t button_pin);
bool button_is_pressed();

#endif // BUTTON_H