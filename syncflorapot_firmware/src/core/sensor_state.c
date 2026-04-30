#include "sensor_state.h"
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "SENSOR_STATE";

static plant_data_t g_sensor_data = {0};
static SemaphoreHandle_t g_sensor_mutex = NULL;
static bool s_initialized = false;

void sensor_state_init(void)
{
    if (s_initialized) {
        ESP_LOGW(TAG, "Sensor state already initialized");
        return;
    }

    g_sensor_mutex = xSemaphoreCreateMutex();
    if (g_sensor_mutex == NULL) {
        ESP_LOGE(TAG, "Failed to create mutex");
        return;
    }

    s_initialized = true;
}

void sensor_state_update_light(int light)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Sensor state not initialized");
        return;
    }

    xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);
    g_sensor_data.light = light;
    xSemaphoreGive(g_sensor_mutex);
}

void sensor_state_update_water(float water_level)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Sensor state not initialized");
        return;
    }

    xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);
    g_sensor_data.water_level = water_level;
    xSemaphoreGive(g_sensor_mutex);
}

void sensor_state_update_soil(int soil_moisture)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Sensor state not initialized");
        return;
    }

    xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);
    g_sensor_data.soil_moisture = soil_moisture;
    xSemaphoreGive(g_sensor_mutex);
}

void sensor_state_update_pump(bool pump_state)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Sensor state not initialized");
        return;
    }

    xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);
    g_sensor_data.pump_state = pump_state;
    xSemaphoreGive(g_sensor_mutex);
}

plant_data_t sensor_state_get_copy(void)
{
    plant_data_t copy = {0};

    if (!s_initialized) {
        ESP_LOGE(TAG, "Sensor state not initialized");
        return copy;
    }
    
    xSemaphoreTake(g_sensor_mutex, portMAX_DELAY);
    copy = g_sensor_data;
    xSemaphoreGive(g_sensor_mutex);
    return copy;
}