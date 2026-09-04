#include "srv_display.h"
#include "esp_log.h"
#include "core/sensor_state.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "drivers/display/display.h"
#include "dianui/dianui.h"
#include "dianui/core/dianui_types.h"
#include "dianui/core/dianui_renderer.h"
#include "core/button_state.h"
#include "views/views.h"

#define FRAME_RATE_MS 33 // 30fps

static const char *TAG = "SRV_DISPLAY";

// HAL Funcs
static void _set_contrast(uint8_t contrast) { display_set_contrast(contrast); }
static void _set_hline(int x1, int x2, int y, DianUI_Color color) { display_set_hline(x1, x2, y, color == DIANUI_WHITE); }
static void _set_vline(int x, int y1, int y2, DianUI_Color color) { display_set_vline(x, y1, y2, color == DIANUI_WHITE); }
static void _set_pixel(int x, int y, DianUI_Color color) { display_set_pixel(x, y, color == DIANUI_WHITE); }
static void _clear(void) { display_clear(); }
static void _update(void) { display_update(); }
static void _log(const char *level, const char *message) { ESP_LOGI(TAG, "[DianUI %s] %s", level, message); }

static const DianUI_Page *const pages[VIEW_ID_COUNT] = {
    &view_home,
    &view_settings,
    &view_loading,
    &view_about,
    &view_provisionning,
    &view_error
};

// Task
static void srv_display_task(void *pvParameters)
{
    TickType_t xLastWakeTime = xTaskGetTickCount();

    button_event_t btn_event;

    dianui_init_router(pages, VIEW_ID_COUNT, VIEW_ID_LOADING);

    while (1)
    {
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

        while (xQueueReceive(button_event_queue, &btn_event, 0) == pdTRUE)
        {
            switch (btn_event)
            {
            case BTN_EVENT_SINGLE_CLICK:
                dianui_page_input(DIANUI_NAV_NEXT);
                break;
            case BTN_EVENT_LONG_PRESS:
                dianui_page_input(DIANUI_NAV_SELECT);
                break;
            case BTN_EVENT_NONE:
            default:
                break;
            }
        }

        dianui_time_update(now);
        dianui_update_page();
        dianui_render();
        vTaskDelayUntil(&xLastWakeTime, pdMS_TO_TICKS(FRAME_RATE_MS));
    }
}

// Task init
void srv_display_init(void)
{
    display_init();

    static DianUI_HAL display_hal = {
        .set_pixel = _set_pixel,
        .set_hline = _set_hline,
        .set_vline = _set_vline,
        .clear = _clear,
        .update = _update,
        .set_contrast = _set_contrast,
        .log = _log,
        .width = 128,
        .height = 64};

    dianui_init(&display_hal);

    xTaskCreate(srv_display_task, "DisplayTask", 4096, NULL, 5, NULL);

    ESP_LOGI(TAG, "OK");
}