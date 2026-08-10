#include "srv_button.h"
#include "drivers/button/button.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_timer.h"

static const char *TAG = "SRV_BUTTON";

gpio_num_t button_pin = GPIO_NUM_4; 

static void button_task_init(void)
{
    esp_err_t ret = button_init(button_pin);

    if (ret != ESP_OK)
    {
        ESP_LOGE(TAG, "Button init failed: %s", esp_err_to_name(ret));
        vTaskDelete(NULL);
        return;
    }
}

void srv_button_task(void *pvParameters)
{
    button_task_init();

    while (1)
    {
        if (button_is_pressed())
        {
            ESP_LOGI(TAG, "Button pressed!");
        }
        
        vTaskDelay(pdMS_TO_TICKS(100)); 
    }
}