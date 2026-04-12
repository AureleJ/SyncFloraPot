#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "drivers/adc_manager/adc_manager.h"
#include "drivers/ldr/ldr.h"
#include "drivers/moisture/moisture.h"
#include "drivers/ultrasonic/ultrasonic.h"
#include "config.h"

static const char *TAG = "MAIN";

void task_sensors(void *pvParameters);

void task_sensors(void *pvParameters)
{
    esp_err_t ret = ldr_init(LDR_ADC_CHANNEL);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "LDR init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    ret = moisture_init(MOISTURE_ADC_CHANNEL);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Moisture init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    esp_err_t ret = ultrasonic_init(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Ultrasonic init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    while (1)
    {
        int light = ldr_read_percent();
        int moisture = moisture_read_percent();
        float distance = 0.0f;
        ret = ultrasonic_read_distance(&distance);
        if (ret != ESP_OK)
        {
            ESP_LOGE(TAG, "Failed to read ultrasonic distance: %s", esp_err_to_name(ret));
            vTaskDelay(pdMS_TO_TICKS(1000));
            continue;
        }

        ESP_LOGI(TAG, "Light: %d%% | Moisture: %d%% | Distance: %.2f cm", light, moisture, distance);
        vTaskDelay(pdMS_TO_TICKS(1000));
    }
}

void app_main(void)
{
    esp_err_t ret = adc_manager_init();
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "ADC manager init failed: %s", esp_err_to_name(ret));
        return;
    }

    xTaskCreate(task_sensors, "task_sensors", 4096, NULL, 5, NULL);
}