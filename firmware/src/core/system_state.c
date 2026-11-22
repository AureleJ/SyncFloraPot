#include "system_state.h"
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/semphr.h"
#include "esp_log.h"

static const char *TAG = "SYSTEM_STATE";

static system_state_t current_state = SYS_STATE_BOOT;
static SemaphoreHandle_t g_state_mutex = NULL;
static EventGroupHandle_t sys_event_group = NULL;
static bool s_initialized = false;

void system_state_init(void)
{
    if (s_initialized)
    {
        ESP_LOGW(TAG, "System state already initialized");
        return;
    }

    g_state_mutex = xSemaphoreCreateMutex();
    sys_event_group = xEventGroupCreate();
    if (g_state_mutex == NULL)
    {
        ESP_LOGE(TAG, "Failed to create mutex");
        return;
    }

    s_initialized = true;

    ESP_LOGI(TAG, "OK");
}

void system_state_set(system_state_t state)
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "System state not initialized");
        return;
    }

    xSemaphoreTake(g_state_mutex, portMAX_DELAY);
    if (current_state != state)
    {
        ESP_LOGI(TAG, "Transition d'état : %d -> %d", current_state, state);
        current_state = state;

        if (state == SYS_STATE_CONNECTED)
        {
            xEventGroupSetBits(sys_event_group, SYS_EVT_WIFI_CONNECTED);
        }
        else
        {
            xEventGroupClearBits(sys_event_group, SYS_EVT_WIFI_CONNECTED);
        }
    }
    xSemaphoreGive(g_state_mutex);
}

system_state_t system_state_get()
{
    if (!s_initialized)
    {
        ESP_LOGE(TAG, "System state not initialized");
        return SYS_STATE_ERROR;
    }

    system_state_t state;
    xSemaphoreTake(g_state_mutex, portMAX_DELAY);
    state = current_state;
    xSemaphoreGive(g_state_mutex);
    return state;
}

EventGroupHandle_t system_state_get_event_group()
{
    return sys_event_group;
}