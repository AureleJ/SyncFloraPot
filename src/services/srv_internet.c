#include "srv_internet.h"
#include "freertos/task.h"
#include "core/sensor_state.h"
#include "time.h"
#include "esp_log.h"
#include "core/message.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "network/wifi_manager.h"

static const char *TAG = "SRV_INTERNET";

void srv_internet_task(void *pvParameters)
{
    wifi_init_sta(); 

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = "mqtt://192.168.1.26",
        .broker.address.port = 1883,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    
    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "Client MQTT lancé, attente de connexion...");

    while (1)
    {
        ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        plant_data_t data = sensor_state_get_copy();

        char json_data[256];
        snprintf(json_data, sizeof(json_data),
                 "{\"timestamp\": %lld, \"soil_moisture\": %d, \"light\": %d, \"water_level\": %.1f, \"pump_state\": %s}",
                 time(NULL),
                 data.soil_moisture,
                 data.light,
                 data.water_level,
                 data.pump_state ? "true" : "false");

        esp_mqtt_client_publish(client, "florapot/sensors", json_data, 0, 1, 0);
    }
}