#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_log.h"
#include "drivers/adc_manager/adc_manager.h"
#include "core/sensor_state.h"
#include "core/message.h"
#include "services/srv_monitoring.h"
#include "services/srv_internet.h"
#include "services/srv_display.h"
#include "core/handles.h"
#include "drivers/i2c_manager/i2c_manager.h"

static const char *TAG = "MAIN";

QueueHandle_t hydration_queue = NULL;

TaskHandle_t srv_internet_task_handle = NULL;
TaskHandle_t srv_display_task_handle = NULL;

void app_main(void)
{
    ESP_LOGI(TAG, "Starting Smart Irrigation System");

    i2c_manager_init();

    xTaskCreate(srv_display_task, "DisplayTask", 4096, NULL, 5, &srv_display_task_handle);

    ESP_ERROR_CHECK(adc_manager_init());
    sensor_state_init();

    hydration_queue = xQueueCreate(10, sizeof(hydration_msg_t));

    xTaskCreate(srv_monitoring_task, "MonitoringTask", 4096, NULL, 5, NULL);
    xTaskCreate(srv_internet_task, "InternetTask", 4096, NULL, 5, &srv_internet_task_handle);   
}