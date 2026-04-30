#ifndef ADC_MANAGER_H
#define ADC_MANAGER_H

#include "esp_adc/adc_oneshot.h"
#include "esp_err.h"

/**
 * @brief Initialize the ADC manager
 */
esp_err_t adc_manager_init(void);

/**
 * @brief Get the ADC oneshot unit handle for use in sensor drivers
 *
 * @return ADC oneshot unit handle
 */
adc_oneshot_unit_handle_t adc_manager_get_handle(void);

#endif // ADC_MANAGER_H