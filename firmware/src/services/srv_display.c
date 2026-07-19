#include "srv_display.h"
#include "plant_data.h"
#include "esp_log.h"
#include "sensor_state.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_mac.h"
#include "drivers/display/display.h"
#include "dianui/dianui.h"
#include "dianui/core/dianui_types.h"
#include "dianui/core/dianui_renderer.h"
#include "dianui/extensions/dianui_face.h"
#include "math.h"

#define ICON_SIZE 8

// static const uint8_t icon_water[ICON_SIZE] = {0x10, 0x38, 0x7C, 0xFE, 0xFE, 0x7C, 0x38, 0x00};
// static const uint8_t icon_wifi[ICON_SIZE] = {0x80, 0x80, 0xA0, 0xA0, 0xA8, 0xA8, 0xAA, 0xAA};

static const char *TAG = "SRV_DISPLAY";

#define FRAME_RATE_MS 33 // 30 FPS

static void _set_contrast(uint8_t contrast)
{
    display_set_contrast(contrast);
}

static void _set_hline(int x1, int x2, int y, DianUI_Color color)
{
    display_set_hline(x1, x2, y, color == DIANUI_WHITE);
}

static void _set_pixel(int x, int y, DianUI_Color color)
{
    if (color == DIANUI_WHITE)
    {
        display_set_pixel(x, y, true);
    }
    else
    {
        display_set_pixel(x, y, false);
    }
}

static void _log(const char *level, const char *message)
{
    if (strcmp(level, "ERROR") == 0)
    {
        ESP_LOGE(TAG, "%s", message);
    }
    else if (strcmp(level, "WARN") == 0)
    {
        ESP_LOGW(TAG, "%s", message);
    }
    else
    {
        ESP_LOGI(TAG, "%s", message);
    }
}

static void _clear(void)
{
    display_clear();
}

static void _update(void)
{
    display_update();
}

void splash_page()
{
    DianUI_TextElement *splashTitle = dianui_create_text(0, 0, 128, 64, DIANUI_CENTER, DIANUI_CENTER, "FloraPot", DIANUI_WHITE);
    DianUI_Animation *splashTitleAnim = dianui_animate_element((DianUI_BaseElement *)splashTitle, DIANUI_TRANSLATE_Y, 1000, 0, -30, 0, false, DIANUI_EASE_OUT);
    dianui_start_animation(splashTitleAnim, xTaskGetTickCount() * portTICK_PERIOD_MS);

    while (!dianui_is_animation_complete(splashTitleAnim))
    {
        dianui_update_animations(xTaskGetTickCount() * portTICK_PERIOD_MS);
        dianui_render();
        vTaskDelay(pdMS_TO_TICKS(FRAME_RATE_MS));
    }
}

DianUI_TextElement *soilMoistureElement = NULL;
DianUI_TextElement *lightElement = NULL;
DianUI_TextElement *waterLevelElement = NULL;

void main_page()
{
    dianui_face_init();
    // DianUI_CircleElement *circle = dianui_create_shape_circle(64, 32, 20, DIANUI_CENTER, DIANUI_CENTER, DIANUI_WHITE);

    /* DianUI_IconElement *wifiIconElement = dianui_create_icon(0, 0, 8, 8, DIANUI_RIGHT, DIANUI_CENTER, icon_wifi, ICON_SIZE);
    soilMoistureElement = dianui_create_text(0, 10, 128, 10, DIANUI_LEFT, DIANUI_CENTER, "Soil: --%", DIANUI_WHITE);
    lightElement = dianui_create_text(0, 20, 128, 10, DIANUI_LEFT, DIANUI_CENTER, "Light: --%", DIANUI_WHITE);
    waterLevelElement = dianui_create_text(0, 30, 128, 10, DIANUI_LEFT, DIANUI_CENTER, "Water: --cm", DIANUI_WHITE);

    dianui_add_icon_to_text(soilMoistureElement, icon_water, ICON_SIZE, 3);
    dianui_add_icon_to_text(lightElement, icon_water, ICON_SIZE, 3);
    dianui_add_icon_to_text(waterLevelElement, icon_water, ICON_SIZE, 3);
    */
}

void srv_display_task(void *pvParameters)
{
    display_init();

    DianUI_HAL display_hal = {
        .set_pixel = _set_pixel,
        .set_hline = _set_hline,
        .clear = _clear,
        .update = _update,
        .set_contrast = _set_contrast,
        .log = _log,
        .width = 128,
        .height = 64};

    dianui_init(&display_hal);

    DianUI_Page splashPage = {
        .name = "splash",
        .on_load = splash_page};

    DianUI_Page mainPage = {
        .name = "main",
        .on_load = main_page};

    dianui_load_page(&splashPage);

    vTaskDelay(pdMS_TO_TICKS(1000));

    dianui_load_page(&mainPage);

    while (1)
    {
        uint32_t now = xTaskGetTickCount() * portTICK_PERIOD_MS;

        plant_data_t data = sensor_state_get_copy();

        if (data.light <= 15)
        {
            dianui_set_emotion(SLEEPY);
        }
        else
        {
            dianui_set_emotion(IDLE);
        }

        dianui_update_face(now);
        dianui_update_animations(now);
        dianui_render();
        vTaskDelay(pdMS_TO_TICKS(FRAME_RATE_MS));
    }

    /* static char soilBuf[32];
    static char lightBuf[32];
    static char waterBuf[32];

    while (1)
    {
        uint32_t notification = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(FRAME_RATE_MS));

        if (notification > 0)
        {

            snprintf(soilBuf, sizeof(soilBuf), "Soil: %d%%", data.soil_moisture);
            dianui_update_text(soilMoistureElement, soilBuf);

            snprintf(lightBuf, sizeof(lightBuf), "Light: %d%%", data.light);
            dianui_update_text(lightElement, lightBuf);

            snprintf(waterBuf, sizeof(waterBuf), "Water: %.1fcm", data.water_level);
            dianui_update_text(waterLevelElement, waterBuf);
        }

        dianui_update_animations(xTaskGetTickCount() * portTICK_PERIOD_MS);
        dianui_render();
        vTaskDelay(pdMS_TO_TICKS(FRAME_RATE_MS));
    } */
}