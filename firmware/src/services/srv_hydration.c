#include "srv_hydration.h"
#include "core/sensor_state.h"
#include "core/message.h"
#include "esp_log.h"
#include "drivers/pump/pump.h"
#include "config.h"

static const char *TAG = "SRV_HYDRATION";
QueueHandle_t hydration_queue = NULL;

// Task
static void srv_hydration_task(void *pvParameters)
{
    hydration_msg_t msg;

    while (1)
    {
        xQueueReceive(hydration_queue, &msg, portMAX_DELAY);

        switch (msg.command)
        {
            case CMD_WATER_DURATION:
            {
                if (pump_on() != ESP_OK)
                {
                    ESP_LOGW(TAG, "Pump unavailable");
                    break;
                }
                sensor_state_update_pump(true);

                uint32_t elapsed = 0;
                while (elapsed < msg.duration_ms && pump_get_state() == PUMP_STATE_WATERING)
                {
                    hydration_msg_t tmp_msg;
                    BaseType_t result = xQueueReceive(hydration_queue, &tmp_msg, pdMS_TO_TICKS(100));
                    if (result == pdPASS && tmp_msg.command == CMD_WATER_STOP)
                    {
                        ESP_LOGI(TAG, "Watering stopped by command");
                        break;
                    }

                    elapsed += 100;
                }

                pump_off();
                sensor_state_update_pump(false);
                break;
            }
            case CMD_WATER_MOISTURE:
            {
                if (pump_on() != ESP_OK)
                {
                    ESP_LOGW(TAG, "Pump unavailable");
                    break;
                }
                sensor_state_update_pump(true);

                while (pump_get_state() == PUMP_STATE_WATERING)
                {
                    plant_data_t data = sensor_state_get_copy();

                    if (data.soil_moisture >= msg.target_moisture)
                        break;

                    hydration_msg_t tmp_msg;
                    BaseType_t result = xQueueReceive(hydration_queue, &tmp_msg, pdMS_TO_TICKS(100));
                    if (result == pdPASS && tmp_msg.command == CMD_WATER_STOP)
                    {
                        ESP_LOGI(TAG, "Watering stopped by command");
                        break;
                    }
                }

                pump_off();
                sensor_state_update_pump(false);
                break;
            }
            case CMD_WATER_STOP:
            {
                pump_off();
                sensor_state_update_pump(false);
                break;
            }
        }
    }
}

// Task init
void srv_hydration_init(void)
{
    esp_err_t ret = pump_init(PUMP_GPIO_PIN, PUMP_WATERING_MAX_DURATION_MS, PUMP_WATERING_COOLDOWN_MS);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Pump init failed: %s", esp_err_to_name(ret));
    }

    hydration_queue = xQueueCreate(10, sizeof(hydration_msg_t));

    xTaskCreate(srv_hydration_task, "HydrationTask", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "OK");
}
