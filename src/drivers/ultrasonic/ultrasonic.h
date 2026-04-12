#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "driver/gpio.h"
#include "esp_err.h"

esp_err_t ultrasonic_init(gpio_num_t trig_pin, gpio_num_t echo_pin);
esp_err_t ultrasonic_read_distance(float *distance);

#endif // ULTRASONIC_H