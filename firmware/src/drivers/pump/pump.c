#include "pump.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "PUMP_DRIVER";

static gpio_num_t pump_pin = -1;
static esp_timer_handle_t s_max_timer;
static esp_timer_handle_t s_cooldown_timer;
static uint32_t s_max_duration_ms;
static uint32_t s_cooldown_ms;
static pump_state_t pump_state = PUMP_STATE_IDLE;

static bool s_initialized = false;

static void pump_max_timer_cb(void *arg)
{
    ESP_LOGW(TAG, "Max watering duration reached, turning pump off");
    pump_off();
}

static void pump_cooldown_timer_cb(void *arg)
{
    pump_state = PUMP_STATE_IDLE;
}

esp_err_t pump_init(gpio_num_t pump_pin_arg, uint32_t max_duration_ms, uint32_t cooldown_ms)
{
    pump_pin = pump_pin_arg;
    pump_state = PUMP_STATE_IDLE;
    s_max_duration_ms = max_duration_ms;
    s_cooldown_ms = cooldown_ms;

    esp_err_t ret = gpio_set_direction(pump_pin, GPIO_MODE_OUTPUT);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set pump pin direction: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = gpio_set_level(pump_pin, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set pump pin level: %s", esp_err_to_name(ret));
        return ret;
    }

    const esp_timer_create_args_t max_args = {
        .callback = &pump_max_timer_cb,
        .name = "pump_max_timer",
    };
    ret = esp_timer_create(&max_args, &s_max_timer);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create max-duration timer: %s", esp_err_to_name(ret));
        return ret;
    }

    const esp_timer_create_args_t cooldown_args = {
        .callback = &pump_cooldown_timer_cb,
        .name = "pump_cooldown_timer",
    };
    ret = esp_timer_create(&cooldown_args, &s_cooldown_timer);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to create cooldown timer: %s", esp_err_to_name(ret));
        return ret;
    }

    s_initialized = true;

    return ESP_OK;
}

esp_err_t pump_on(void)
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "Pump not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (pump_state != PUMP_STATE_IDLE)
    {
        ESP_LOGW(TAG, "Pompe indisponible (etat=%d), demande ignoree", (int)pump_state);
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t ret = gpio_set_level(pump_pin, 1);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to turn pump on: %s", esp_err_to_name(ret));
        return ret;
    }

    pump_state = PUMP_STATE_WATERING;
    esp_timer_start_once(s_max_timer, (uint64_t)s_max_duration_ms * 1000);

    return ESP_OK;
}

esp_err_t pump_off(void)
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "Pump not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    if (pump_state != PUMP_STATE_WATERING)
    {
        return ESP_OK;
    }

    esp_err_t ret = gpio_set_level(pump_pin, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to turn pump off: %s", esp_err_to_name(ret));
        return ret;
    }

    esp_timer_stop(s_max_timer);

    pump_state = PUMP_STATE_COOLDOWN;
    esp_timer_start_once(s_cooldown_timer, (uint64_t)s_cooldown_ms * 1000);

    return ESP_OK;
}

pump_state_t pump_get_state(void)
{
    return pump_state;
}