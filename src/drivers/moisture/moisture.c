#include "moisture.h"
#include "esp_adc/adc_oneshot.h"
#include "esp_log.h"
#include "../adc_manager/adc_manager.h"

static const char *TAG = "MOISTURE_DRIVER";
static adc_channel_t moisture_adc_channel = -1;
static bool s_initialized = false;

esp_err_t moisture_init(adc_channel_t adc_channel)
{
    moisture_adc_channel = adc_channel;

    adc_oneshot_unit_handle_t adc_handle = adc_manager_get_handle();

    adc_oneshot_chan_cfg_t chan_config = {
        .atten = ADC_ATTEN_DB_12,
        .bitwidth = ADC_BITWIDTH_12,
    };

    esp_err_t ret = adc_oneshot_config_channel(adc_handle, moisture_adc_channel, &chan_config);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC channel config failed: %s", esp_err_to_name(ret));
        return ret;
    }

    s_initialized = true;

    return ESP_OK;
}

int moisture_read_percent(void)
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "Moisture sensor not initialized");
        return -1;
    }

    adc_oneshot_unit_handle_t adc_handle = adc_manager_get_handle();

    int total = 0;
    for (int i = 0; i < 10; i++)
    {
        int raw = 0;
        esp_err_t ret = adc_oneshot_read(adc_handle, moisture_adc_channel, &raw);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "ADC read failed: %s", esp_err_to_name(ret));
            return -1;
        }
        total += raw;
    }

    int average = total / 10;
    return (average * 100) / 4095;
}