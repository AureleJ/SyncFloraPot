#include "ultrasonic.h"
#include "esp_err.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include "esp_timer.h"
#include "rom/ets_sys.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define TRIGGER_LOW_DELAY 4
#define TRIGGER_HIGH_DELAY 10
#define PING_TIMEOUT 6000

static const char *TAG = "ULTRASONIC";
static gpio_num_t ultrasonic_trig_pin = -1;
static gpio_num_t ultrasonic_echo_pin = -1;
static bool s_initialized = false;

esp_err_t ultrasonic_init(gpio_num_t trig_pin, gpio_num_t echo_pin)
{
    ultrasonic_trig_pin = trig_pin;
    ultrasonic_echo_pin = echo_pin;

    esp_err_t err = gpio_set_direction(ultrasonic_trig_pin, GPIO_MODE_OUTPUT);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin direction: %s", esp_err_to_name(err));
        return err;
    }

    err = gpio_set_direction(ultrasonic_echo_pin, GPIO_MODE_INPUT);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set echo pin direction: %s", esp_err_to_name(err));
        return err;
    }

    err = gpio_set_level(ultrasonic_trig_pin, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin level: %s", esp_err_to_name(err));
        return err;
    }

    s_initialized = true;

    return ESP_OK;
}

esp_err_t ultrasonic_read_distance(float *distance)
{
    if (distance == NULL)
    {
        ESP_LOGE(TAG, "Distance pointer is NULL");
        return ESP_ERR_INVALID_ARG;
    }

    if (!s_initialized)
    {
        ESP_LOGE(TAG, "Ultrasonic sensor not initialized");
        return ESP_ERR_INVALID_STATE;
    }

    esp_err_t err = gpio_set_level(ultrasonic_trig_pin, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin level: %s", esp_err_to_name(err));
        return err;
    }

    ets_delay_us(TRIGGER_LOW_DELAY);

    err = gpio_set_level(ultrasonic_trig_pin, 1);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin level: %s", esp_err_to_name(err));
        return err;
    }

    ets_delay_us(TRIGGER_HIGH_DELAY);

    err = gpio_set_level(ultrasonic_trig_pin, 0);
    if (err != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin level: %s", esp_err_to_name(err));
        return err;
    }

    if (gpio_get_level(ultrasonic_echo_pin)) 
    {
        ESP_LOGE(TAG, "Echo pin is high after triggering, sensor may be malfunctioning");
        return ESP_FAIL;
    }
    
    int64_t start_time = esp_timer_get_time();

    while (!gpio_get_level(ultrasonic_echo_pin))
    {
        if (esp_timer_get_time() - start_time > PING_TIMEOUT)
        {
            ESP_LOGE(TAG, "Ping timeout waiting for echo to go high");
            return ESP_ERR_TIMEOUT;
        }
    }

    int64_t echo_start = esp_timer_get_time();
    int64_t time = echo_start;

    while (gpio_get_level(ultrasonic_echo_pin))
    {
        time = esp_timer_get_time();
        if (time - echo_start > PING_TIMEOUT)
        {
            ESP_LOGE(TAG, "Ping timeout waiting for echo to go low");
            return ESP_ERR_TIMEOUT;   
        }
    }

    int64_t echo_time = time - echo_start;
    *distance = (echo_time / 2.0f) * 0.0343f; 

    return ESP_OK;
}