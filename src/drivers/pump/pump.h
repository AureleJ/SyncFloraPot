#ifndef PUMP_H
#define PUMP_H

#include "driver/gpio.h"
#include "esp_err.h"

typedef enum {
    PUMP_STATE_IDLE,
    PUMP_STATE_WATERING,
    PUMP_STATE_COOLDOWN
} pump_state_t;

esp_err_t pump_init(gpio_num_t pump_pin);
esp_err_t pump_on();
esp_err_t pump_off();

#endif // PUMP_H