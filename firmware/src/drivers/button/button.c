#include "button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "BUTTON_DRIVER";
static gpio_num_t button_pin = -1;
static bool s_initialized = false;

esp_err_t button_init(gpio_num_t button_pin_arg)
{
    button_pin = button_pin_arg;

    gpio_config_t io_conf = {
        .pin_bit_mask = (1ULL << button_pin),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_ENABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_NEGEDGE
    };

    gpio_config(&io_conf);

    esp_err_t ret = gpio_set_direction(button_pin, GPIO_MODE_INPUT);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set button pin direction: %s", esp_err_to_name(ret));
        return ret;
    }

    s_initialized = true;

    return ESP_OK;
}

uint32_t currentPressTime = 0;
uint32_t lastPressTime = 0;
bool button_pressed = false;

bool button_is_pressed()
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "Button not initialized");
        return false;
    }

    currentPressTime = esp_timer_get_time() / 1000;

    if (currentPressTime - lastPressTime > 30)
    {
        lastPressTime = currentPressTime;
        button_pressed = (gpio_get_level(button_pin) == 0);
    }

    return button_pressed;
}