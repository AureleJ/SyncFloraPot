#include "srv_hydration.h"
#include "core/sensor_state.h"
#include "core/message.h"
#include "esp_log.h"
#include "drivers/pump/pump.h"
#include "config.h"

static const char *TAG = "SRV_HYDRATION";

static void hydration_init(void)
{
    esp_err_t ret = pump_init(PUMP_GPIO_PIN);
    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Pump init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }
}

static void start_watering_cycle(int duration_ms)
{
    pump_on();
    sensor_state_update_pump(true);
    vTaskDelay(pdMS_TO_TICKS(duration_ms));
    pump_off();
    sensor_state_update_pump(false);
}

static void stop_watering_cycle(void)
{
    pump_off();
    sensor_state_update_pump(false);
}

void hydration_task(void *pvParameters)
{
    hydration_init();

    hydration_msg_t received_message;

    while (1)
    {
        xQueueReceive(hydration_queue, &received_message, portMAX_DELAY);

        switch (received_message.command)
        {
        case CMD_WATER_START:
            start_watering_cycle(received_message.duration_ms);
            break;

        case CMD_WATER_STOP:
            stop_watering_cycle();
            break;

        default:
            ESP_LOGW(TAG, "Received unknown command: %d", received_message.command);
            break;
        }
    }
}