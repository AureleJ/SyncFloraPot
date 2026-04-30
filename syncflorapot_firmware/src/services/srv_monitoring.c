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
#include "core/handles.h"

static const char *TAG = "SRV_MONITORING";

static void monitoring_init(void)
{
    esp_err_t ret = ultrasonic_init(ULTRASONIC_TRIG_PIN, ULTRASONIC_ECHO_PIN);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Ultrasonic init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    ret = ldr_init(LDR_ADC_CHANNEL);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "LDR init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }

    ret = moisture_init(MOISTURE_ADC_CHANNEL, MOISTURE_POWER_GPIO);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Moisture init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }
}

void srv_monitoring_task(void *pvParameters)
{
    monitoring_init();

    while (1)
    {
        int light = ldr_read_percent();
        float water = ultrasonic_read_distance();
        int soil = moisture_read_percent();

        ESP_LOGI(TAG, "Sensor readings - Light: %d%%, Water: %.2f cm, Soil: %d%%", light, water, soil);

        if (soil < MOISTURE_THRESHOLD) {
            hydration_msg_t message_a_envoyer;
            message_a_envoyer.command = CMD_WATER_START;
            message_a_envoyer.duration_ms = 5000;

            if (xQueueSend(hydration_queue, &message_a_envoyer, pdMS_TO_TICKS(100)) == pdPASS) {
                ESP_LOGI(TAG, "Sent watering command to queue");
            } else {
                ESP_LOGE(TAG, "Failed to send watering command to queue");
            }

            water = ultrasonic_read_distance();

            if (water < WATER_LEVEL_THRESHOLD) {
                ESP_LOGE(TAG, "Water level low after watering, water level: %.2f cm", water);
            } else {
                ESP_LOGI(TAG, "Watering successful, water level: %.2f cm", water);
            }
        } 

        sensor_state_update_light(light);
        sensor_state_update_water(water);
        sensor_state_update_soil(soil);

        if (srv_internet_task_handle != NULL) {
            xTaskNotifyGive(srv_internet_task_handle);
        } else {
            ESP_LOGW(TAG, "Handle de la tâche Internet non disponible");
        }

        if (srv_display_task_handle != NULL) {
            xTaskNotifyGive(srv_display_task_handle);
        } else {
            ESP_LOGW(TAG, "Handle de la tâche Display non disponible");
        }

        vTaskDelay(pdMS_TO_TICKS(MONITORING_INTERVAL_MS));
    }
}