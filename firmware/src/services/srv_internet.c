#include "srv_internet.h"
#include "core/system_state.h"
#include "core/message.h"
#include "freertos/task.h"
#include "core/sensor_state.h"
#include "time.h"
#include "string.h"
#include "stdio.h"
#include "cJSON.h"
#include "esp_log.h"
#include "mqtt_client.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "esp_wifi.h"
#include "config.h"
#include "esp_crt_bundle.h"
#include "secrets.h" // non versionne, voir secrets.h.example

static const char *TAG = "SRV_INTERNET";
static char receive_topic[128];
static char send_topic[128];
static char device_name[32];

static void mqtt_event_handler(void *handler_args, esp_event_base_t base, int32_t event_id, void *event_data)
{
    esp_mqtt_event_handle_t event = event_data;

    if (event_id == MQTT_EVENT_CONNECTED)
    {
        int message_id = esp_mqtt_client_subscribe(event->client, receive_topic, 1);
        ESP_LOGI(TAG, "Abonnement aux commandes: %s (message_id=%d)", receive_topic, message_id);
        return;
    }

    if (event_id != MQTT_EVENT_DATA || event->topic_len != strlen(receive_topic) || strncmp(event->topic, receive_topic, event->topic_len) != 0)
    {
        return;
    }

    cJSON *root = cJSON_ParseWithLength(event->data, event->data_len);
    if (root == NULL)
    {
        ESP_LOGW(TAG, "Commande MQTT JSON invalide");
        return;
    }

    cJSON *command = cJSON_GetObjectItemCaseSensitive(root, "command");
    hydration_msg_t message = {0};
    bool valid = false;

    if (cJSON_IsString(command) && command->valuestring != NULL)
    {
        if (strcmp(command->valuestring, "water_duration") == 0)
        {
            cJSON *duration = cJSON_GetObjectItemCaseSensitive(root, "duration_ms");

            if (cJSON_IsNumber(duration) && duration->valuedouble > 0 && duration->valuedouble <= 120000)
            {
                message.command = CMD_WATER_DURATION;
                message.duration_ms = (uint32_t)duration->valuedouble;
                valid = true;
            }
        }
        else if (strcmp(command->valuestring, "water_moisture") == 0)
        {
            cJSON *target = cJSON_GetObjectItemCaseSensitive(root, "target_moisture");
            cJSON *max_duration = cJSON_GetObjectItemCaseSensitive(root, "duration_ms");

            if (cJSON_IsNumber(target) && cJSON_IsNumber(max_duration) &&
                target->valuedouble >= 0 && target->valuedouble <= 100 &&
                max_duration->valuedouble > 0 && max_duration->valuedouble <= 120000)
            {
                message.command = CMD_WATER_MOISTURE;
                message.target_moisture = (uint8_t)target->valuedouble;
                message.duration_ms = (uint32_t)max_duration->valuedouble;
                valid = true;
            }
        }
        else if (strcmp(command->valuestring, "water_stop") == 0)
        {
            message.command = CMD_WATER_STOP;
            valid = true;
        }
    }

    if (valid && xQueueSend(hydration_queue, &message, pdMS_TO_TICKS(100)) != pdTRUE)
    {
        ESP_LOGW(TAG, "Queue d'arrosage pleine");
    }
    else if (!valid)
    {
        ESP_LOGW(TAG, "Commande d'arrosage inconnue ou invalide");
    }

    cJSON_Delete(root);
}

// Task
static void srv_internet_task(void *pvParameters)
{
    EventGroupHandle_t sys_evt = system_state_get_event_group();

    uint8_t eth_mac[6];
    ESP_ERROR_CHECK(esp_wifi_get_mac(WIFI_IF_STA, eth_mac));

    snprintf(device_name, sizeof(device_name), "PROV_%02X%02X%02X", eth_mac[0], eth_mac[1], eth_mac[2]);
    snprintf(receive_topic, sizeof(receive_topic), "syncflorapot/pots/%s/commands", device_name);
    snprintf(send_topic, sizeof(send_topic), "syncflorapot/pots/%s", device_name);

    esp_mqtt_client_config_t mqtt_cfg = {
        .broker.address.uri = MQTT_BROKER_URI,
        .broker.verification.crt_bundle_attach = esp_crt_bundle_attach,
        .credentials.username = MQTT_USERNAME,
        .credentials.authentication.password = MQTT_PASSWORD,
    };

    esp_mqtt_client_handle_t client = esp_mqtt_client_init(&mqtt_cfg);
    ESP_ERROR_CHECK(esp_mqtt_client_register_event(client, ESP_EVENT_ANY_ID, mqtt_event_handler, NULL));

    esp_mqtt_client_start(client);

    ESP_LOGI(TAG, "Client MQTT lancé, attente de connexion...");

    while (1)
    {
        EventBits_t bits = xEventGroupWaitBits(
            sys_evt,
            SYS_EVT_WIFI_CONNECTED,
            pdFALSE,
            pdTRUE,
            portMAX_DELAY);

        if (bits & SYS_EVT_WIFI_CONNECTED)
        {
            plant_data_t data = sensor_state_get_copy();

            char json_data[256];
            snprintf(json_data, sizeof(json_data),
                     "{\"soil_moisture\": %d, \"light\": %d, \"water_level\": %d, \"pump_state\": %d}",
                     data.soil_moisture,
                     data.light,
                     data.water_level,
                     (int)data.pump_state);

            esp_mqtt_client_publish(client, send_topic, json_data, 0, 1, 0);
            ESP_LOGI(TAG, "Published data to MQTT: %s on topic: %s", json_data, send_topic);
        }

        vTaskDelay(pdMS_TO_TICKS(MQTT_PUBLISH_INTERVAL_MS));
    }
}

// Task init
void srv_internet_init(void)
{
    xTaskCreate(srv_internet_task, "InternetTask", 4096, NULL, 5, NULL);
    ESP_LOGI(TAG, "OK");
}