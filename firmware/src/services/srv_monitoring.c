#include "srv_monitoring.h"
#include "core/sensor_state.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "drivers/ldr/ldr.h"
#include "drivers/ultrasonic/ultrasonic.h"
#include "drivers/moisture/moisture.h"
#include "config.h"
#include "core/message.h"
#include "srv_internet.h"

static const char *TAG = "SRV_MONITORING";

static uint32_t last_watering_time = 0;

// Task
static void srv_monitoring_task(void *pvParameters)
{
    while (1)
    {
        int light = ldr_read_percent();
        float water = ultrasonic_read_distance();
        int soil = moisture_read_percent();

        int water_percent = 0;
        
        if (water < WATER_LEVEL_MAX_THRESHOLD)
        {
            water = WATER_LEVEL_MAX_THRESHOLD;
        }
        else if (water > WATER_LEVEL_MIN_THRESHOLD)
        {
            water = WATER_LEVEL_MIN_THRESHOLD;
        }

        water_percent = (int)((1.0f - (water - WATER_LEVEL_MAX_THRESHOLD) / (WATER_LEVEL_MIN_THRESHOLD - WATER_LEVEL_MAX_THRESHOLD)) * 100.0f);

        ESP_LOGI(TAG, "Light: %d%%, Water: %d%%, Soil: %d%%", light, water_percent, soil);

        sensor_state_update_telemetry(light, water_percent, soil);

        if ((xTaskGetTickCount() - last_watering_time) > pdMS_TO_TICKS(PUMP_WATERING_COOLDOWN_MS) && soil < MOISTURE_DRY_THRESHOLD && !sensor_state_is_pump_on())
        {
            hydration_msg_t message = {
                .command = CMD_WATER_MOISTURE,
                .target_moisture = MOISTURE_TARGET_THRESHOLD
            };

            if (water_percent >= 10)
            {
                BaseType_t queue_result = xQueueSend(hydration_queue, &message, pdMS_TO_TICKS(100));
                if (queue_result == pdPASS)
                {
                    ESP_LOGI(TAG, "Sent watering command to queue");
                    last_watering_time = xTaskGetTickCount();
                }
                else
                {
                    ESP_LOGE(TAG, "Failed to send watering command to queue: error %d", queue_result);
                }
            }
            else
            {
                ESP_LOGE(TAG, "Water level too low to start watering, water level: %d%%", water_percent);
            }
        }

        vTaskDelay(pdMS_TO_TICKS(MONITORING_INTERVAL_MS));
    }
}

// Task init
void srv_monitoring_init(void)
{
    esp_err_t ret = ultrasonic_init(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Ultrasonic init failed: %s", esp_err_to_name(ret));
    }

    ret = ldr_init(LDR_ADC_CHANNEL);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "LDR init failed: %s", esp_err_to_name(ret));
    }

    ret = moisture_init(MOISTURE_ADC_CHANNEL, MOISTURE_POWER_GPIO);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Moisture init failed: %s", esp_err_to_name(ret));
    }

    xTaskCreate(srv_monitoring_task, "MonitoringTask", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "OK");
}