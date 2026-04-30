#include "adc_manager.h"
#include "esp_log.h"
#include <stdbool.h>

static adc_oneshot_unit_handle_t adc_handle;
static const char *TAG = "ADC_MANAGER";
static bool s_initialized = false;

esp_err_t adc_manager_init(void)
{
    if (s_initialized)
    {
        ESP_LOGW(TAG, "ADC manager already initialized");
        return ESP_OK;
    }

    adc_oneshot_unit_init_cfg_t config = {
        .unit_id = ADC_UNIT_1,
    };

    esp_err_t ret = adc_oneshot_new_unit(&config, &adc_handle);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC init failed: %s", esp_err_to_name(ret));
        return ret;
    }

    s_initialized = true;
    return ESP_OK;
}

adc_oneshot_unit_handle_t adc_manager_get_handle(void)
{
    return adc_handle;
}