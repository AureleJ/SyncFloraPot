#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "freertos/queue.h"

#include "config.h"

#include "core/system_state.h"
#include "core/sensor_state.h"
#include "network/wifi_manager.h"
#include "drivers/i2c_manager/i2c_manager.h"
#include "drivers/adc_manager/adc_manager.h"

#include "core/message.h"

#include "services/srv_display.h"
#include "services/srv_button.h"
#include "services/srv_monitoring.h"
#include "services/srv_hydration.h"
#include "services/srv_internet.h"

static const char *TAG = "MAIN";

void app_main(void)
{
    ESP_LOGI(TAG, "Start");

    system_state_init();
    sensor_state_init();
    wifi_manager_init();
    i2c_manager_init();
    ESP_ERROR_CHECK(adc_manager_init());
    
    srv_button_init();
    srv_display_init();
    srv_monitoring_init();
    srv_hydration_init();
    srv_internet_init();

    ESP_LOGI(TAG, "Success");
}