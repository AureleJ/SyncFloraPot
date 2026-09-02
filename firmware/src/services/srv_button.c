#include "srv_button.h"
#include "drivers/button/button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"
#include "core/button_state.h"

static const char *TAG = "SRV_BUTTON";
static TaskHandle_t srv_button_task_handle = NULL;
QueueHandle_t button_event_queue = NULL;

static void IRAM_ATTR button_isr_handler(void *arg)
{
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;
    if (srv_button_task_handle != NULL)
    {
        vTaskNotifyGiveFromISR(srv_button_task_handle, &xHigherPriorityTaskWoken);
        portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }
}

void srv_button_task(void *pvParameters)
{
    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        button_event_t event = BTN_EVENT_NONE;

        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

        if (button_is_pressed())
        {
            while (button_is_pressed())
            {
                vTaskDelay(pdMS_TO_TICKS(20));
            }

            uint32_t press_duration = (xTaskGetTickCount() * portTICK_PERIOD_MS) - now;

            if (press_duration >= 500)
            {
                event = BTN_EVENT_LONG_PRESS;
            }
            else if (press_duration >= 50)
            {
                event = BTN_EVENT_SINGLE_CLICK;
            }
        }

        xQueueSend(button_event_queue, &event, 0);
    }
}

void srv_button_init(gpio_num_t button_pin)
{
    button_init(button_pin);

    button_event_queue = xQueueCreate(10, sizeof(button_event_t));

    gpio_install_isr_service(0);
    gpio_isr_handler_add(button_pin, button_isr_handler, NULL);

    xTaskCreate(srv_button_task, "ButtonTask", 2048, NULL, 10, &srv_button_task_handle);

    ESP_LOGI(TAG, "OK");
}