#include "pump.h"

static const char *TAG = "PUMP_DRIVER";
static gpio_num_t pump_pin = -1;
static bool s_initialized = false;

int last_activation_time = 0;

esp_err_t pump_init(gpio_num_t pump_pin_arg)
{
    pump_pin = pump_pin_arg;

    esp_err_t err = gpio_set_direction(pump_pin, GPIO_MODE_OUTPUT);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set pump pin direction: %s", esp_err_to_name(err));
        return err;
    }

    err = gpio_set_level(pump_pin, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set pump pin level: %s", esp_err_to_name(err));
        return err;
    }

    s_initialized = true;

    return ESP_OK;
}

esp_err_t pump_on()
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "Pump not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = gpio_set_level(pump_pin, 1);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to turn pump on: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}

esp_err_t pump_off()
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "Pump not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = gpio_set_level(pump_pin, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to turn pump off: %s", esp_err_to_name(err));
        return err;
    }

    return ESP_OK;
}