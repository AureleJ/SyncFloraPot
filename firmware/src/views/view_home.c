#include "views.h"
#include "dianui/extensions/dianui_face.h"
#include "core/sensor_state.h"

#define ICON_SIZE 8
static const uint8_t icon_wifi[ICON_SIZE] = {0x80, 0x80, 0xA0, 0xA0, 0xA8, 0xA8, 0xAA, 0xAA};

static DianUI_TextElement *timeText = 0;
static DianUI_IconElement *wifiIcon = 0;

static DianUI_TextElement *moistureText = 0;
static DianUI_TextElement *waterLevelText = 0;
static DianUI_TextElement *ldrText = 0;

static void view_home_enter(void)
{
    wifiIcon = dianui_create_icon(0, 0, 128, 8, DIANUI_LEFT, DIANUI_CENTER, icon_wifi, ICON_SIZE);
    timeText = dianui_create_text(0, 0, 128, 8, DIANUI_RIGHT, DIANUI_CENTER, "10:09", DIANUI_WHITE);

    dianui_face_init();

    moistureText = dianui_create_text(0, -8, 128, 64, DIANUI_LEFT, DIANUI_BOTTOM, "--%", DIANUI_WHITE);
    ldrText = dianui_create_text(40, -8, 128, 64, DIANUI_LEFT, DIANUI_BOTTOM, "--%", DIANUI_WHITE);
    waterLevelText = dianui_create_text(90, -8, 128, 64, DIANUI_LEFT, DIANUI_BOTTOM, "--cm", DIANUI_WHITE);
}

static char moisture_buf[16];
static char water_level_buf[16];
static char ldr_buf[16];

static void view_home_update(float dt)
{
    plant_data_t data = sensor_state_get_copy();

    snprintf(water_level_buf, sizeof(water_level_buf), "%dcm", (int)data.water_level);
    dianui_update_text(waterLevelText, water_level_buf);

    snprintf(moisture_buf, sizeof(moisture_buf), "%d%%", data.soil_moisture);
    dianui_update_text(moistureText, moisture_buf);

    snprintf(ldr_buf, sizeof(ldr_buf), "%d%%", data.light);
    dianui_update_text(ldrText, ldr_buf);

    if (data.light <= 15)
    {
        dianui_set_emotion(SLEEPY);
    }
    else if (data.soil_moisture <= 5)
    {
        dianui_set_emotion(ALMOST_DEAD);
    }
    else if (data.soil_moisture <= 15)
    {
        dianui_set_emotion(THIRSTY);
    }
    else
    {
        dianui_set_emotion(IDLE);
    }

    dianui_update_face();
}

static void view_home_action(DianUI_NavEvent action)
{
    if (action == DIANUI_NAV_NEXT)
    {
        dianui_router_push(VIEW_ID_SETTINGS);
    }
}

const DianUI_Page view_home = {
    .name = "Home",
    .on_enter = view_home_enter,
    .on_update = view_home_update,
    .on_input = view_home_action,
    .on_exit = 0,
};