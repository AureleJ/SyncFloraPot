#ifndef MOISTURE_H
#define MOISTURE_H

#include "esp_err.h"

/**
 * @brief Initialize the moisture sensor by configuring the ADC
 *
 * This function sets up the ADC unit and channel for reading the moisture sensor.
 * It should be called before any calls to `moisture_read_percent()`.
 */
esp_err_t moisture_init(int adc_channel);

/**
 * @brief Read moisture level as a percentage from the moisture sensor
 *
 * @return Moisture level as a percentage (0-100)
 */
int moisture_read_percent(void);

#endif // MOISTURE_H