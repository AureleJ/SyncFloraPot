#ifndef ULTRASONIC_H
#define ULTRASONIC_H

#include "esp_err.h"

esp_err_t ultrasonic_init(int trig_pin, int echo_pin);
esp_err_t ultrasonic_read_distance(float *distance);

#endif // ULTRASONIC_H