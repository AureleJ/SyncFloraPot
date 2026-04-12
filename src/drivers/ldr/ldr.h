#ifndef LDR_H
#define LDR_H

#include "driver/adc.h"
#include "esp_err.h"

/**
 * @brief Initialize the LDR sensor by configuring the ADC
 *
 * This function sets up the ADC unit and channel for reading the LDR sensor.
 * It should be called before any calls to `ldr_read_percent()`.
 */
esp_err_t ldr_init(adc_channel_t adc_channel);

/**
 * @brief Read light level as a percentage from the LDR sensor
 *
 * @return Light level as a percentage (0-100)
 */
int ldr_read_percent(void);

#endif // LDR_H