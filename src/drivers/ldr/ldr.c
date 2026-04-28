#include "ldr.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "../adc_manager/adc_manager.h"

static const char *TAG = "LDR_DRIVER";
static adc_channel_t ldr_adc_channel = -1;
static bool s_initialized = false;
static const int ADC_MAX_VALUE = 4095;
static const int NUM_READINGS = 20;

esp_err_t ldr_init(adc_channel_t adc_channel)
{
    ldr_adc_channel = adc_channel;

    adc_oneshot_unit_handle_t adc_handle = adc_manager_get_handle();

    adc_oneshot_chan_cfg_t chan_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    esp_err_t ret = adc_oneshot_config_channel(adc_handle, ldr_adc_channel, &chan_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC channel config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    s_initialized = true;

    return ESP_OK;
}

int ldr_read_percent(void)
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "LDR sensor not initialized");
        return -1;
    }

    adc_oneshot_unit_handle_t adc_handle = adc_manager_get_handle();

    int total = 0;
    int last_raw = 0;
    for (int i = 0; i < NUM_READINGS; i++)
    {
        int raw = 0;
        esp_err_t ret = adc_oneshot_read(adc_handle, ldr_adc_channel, &raw);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ADC read failed: %s", esp_err_to_name(ret));
            return -1;
        }

        if (raw < 0 || raw > ADC_MAX_VALUE)
            continue;

        if (last_raw != 0 && (raw < last_raw / 2 || raw > last_raw * 2))
            continue;

        total += raw;
        last_raw = raw;
    }

    int average = total / NUM_READINGS;
    return (average * 100) / ADC_MAX_VALUE;
}