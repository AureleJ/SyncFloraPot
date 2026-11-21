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
#define PING_TIMEOUT 10000
#define NUM_SAMPLES 10
#define SAMPLE_DELAY_MS 20
#define MIN_DIST_CM 2.0f
#define MAX_DIST_CM 400.0f

static const char *TAG = "ULTRASONIC";
static gpio_num_t ultrasonic_trig_pin = -1;
static gpio_num_t ultrasonic_echo_pin = -1;
static bool s_initialized = false;

esp_err_t ultrasonic_init(gpio_num_t trig_pin, gpio_num_t echo_pin)
{
    ultrasonic_trig_pin = trig_pin;
    ultrasonic_echo_pin = echo_pin;

    esp_err_t ret = gpio_set_direction(ultrasonic_trig_pin, GPIO_MODE_OUTPUT);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin direction: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = gpio_set_direction(ultrasonic_echo_pin, GPIO_MODE_INPUT);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set echo pin direction: %s", esp_err_to_name(ret));
        return ret;
    }

    ret = gpio_set_level(ultrasonic_trig_pin, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin level: %s", esp_err_to_name(ret));
        return ret;
    }

    s_initialized = true;

    return ESP_OK;
}

static float ultrasonic_ping_once(void)
{
    if (gpio_get_level(ultrasonic_echo_pin))
    {
        ESP_LOGW(TAG, "Echo deja haut avant le trigger, echantillon ignore");
        return -1.0f;
    }

    esp_err_t ret = gpio_set_level(ultrasonic_trig_pin, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin level: %s", esp_err_to_name(ret));
        return -1.0f;
    }

    ets_delay_us(TRIGGER_LOW_DELAY);

    ret = gpio_set_level(ultrasonic_trig_pin, 1);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin level: %s", esp_err_to_name(ret));
        return -1.0f;
    }

    ets_delay_us(TRIGGER_HIGH_DELAY);

    ret = gpio_set_level(ultrasonic_trig_pin, 0);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Failed to set trig pin level: %s", esp_err_to_name(ret));
        return -1.0f;
    }

    int64_t start_time = esp_timer_get_time();
    while (!gpio_get_level(ultrasonic_echo_pin))
    {
        if (esp_timer_get_time() - start_time > PING_TIMEOUT) return -1.0f;
    }

    int64_t echo_start = esp_timer_get_time();
    int64_t time = echo_start;

    while (gpio_get_level(ultrasonic_echo_pin))
    {
        time = esp_timer_get_time();
        if (time - echo_start > PING_TIMEOUT)
        {
            break;
        }
    }

    float distance = (time - echo_start) * 0.01715f;

    if (distance < MIN_DIST_CM || distance > MAX_DIST_CM)
    {
        return -1.0f;
    }

    return distance;
}

float ultrasonic_read_distance(void)
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "Ultrasonic sensor not initialized");
        return -1.0f;
    }

    float distances[NUM_SAMPLES] = {0.0f};
    size_t valid_count = 0;

    for (int i = 0; i < NUM_SAMPLES; i++)
    {
        float distance = ultrasonic_ping_once();

        if (distance >= 0.0f)
        {
            distances[valid_count++] = distance;
        }

        vTaskDelay(pdMS_TO_TICKS(SAMPLE_DELAY_MS));
    }

    if (valid_count == 0)
    {
        return -1.0f;
    }

    for (size_t i = 1; i < valid_count; i++)
    {
        float key = distances[i];
        size_t j = i;
        while (j > 0 && distances[j - 1] > key)
        {
            distances[j] = distances[j - 1];
            j--;
        }
        distances[j] = key;
    }

    size_t trim = valid_count / 5;
    float sum = 0.0f;
    size_t count = 0;

    for (size_t i = trim; i < valid_count - trim; i++)
    {
        sum += distances[i];
        count++;
    }

    return sum / (float)count;
}
