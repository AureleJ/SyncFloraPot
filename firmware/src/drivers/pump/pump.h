#ifndef PUMP_H
#define PUMP_H

#include "driver/gpio.h"
#include "esp_err.h"

typedef enum {
    PUMP_STATE_IDLE,
    PUMP_STATE_WATERING,
    PUMP_STATE_COOLDOWN
} pump_state_t;

esp_err_t pump_init(gpio_num_t pump_pin, uint32_t max_duration_ms, uint32_t cooldown_ms);
esp_err_t pump_on(void);
esp_err_t pump_off(void);
pump_state_t pump_get_state(void);

#endif // PUMP_H
