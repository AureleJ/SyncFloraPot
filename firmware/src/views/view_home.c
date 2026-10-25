#include "views.h"
#include "dianui/extensions/dianui_face.h"
#include "core/sensor_state.h"
#include "config.h"
#include <time.h>

#define ICON_SIZE 8
static const uint8_t icon_water_drop[ICON_SIZE] = {0x18, 0x18, 0x3C, 0x2C, 0x4E, 0x4E, 0x7E, 0x3C};
static const uint8_t icon_water_drop_empty[ICON_SIZE] = {0x18, 0x18, 0x24, 0x24, 0x42, 0x42, 0x42, 0x3C};

static DianUI_TextElement *timeText = 0;
static DianUI_IconElement *waterDropIcons[5] = {0};

static void view_home_enter(void)
{
    for (int i = 0; i < 5; i++)
    {
        waterDropIcons[i] = dianui_create_icon(i * (ICON_SIZE + 2), 0, ICON_SIZE, ICON_SIZE, DIANUI_LEFT, DIANUI_BOTTOM, icon_water_drop, ICON_SIZE);
    }

    timeText = dianui_create_text(0, 0, 128, 8, DIANUI_RIGHT, DIANUI_CENTER, "00:00", DIANUI_WHITE);

    dianui_face_init();
}

static int last_water_level = -1;

static void view_home_update(float dt)
{
    plant_data_t data = sensor_state_get_copy();
    
    // Update water level icons
    if (data.water_level != last_water_level)
    {
        int drops = 1 + (data.water_level * 4) / 100;
        if (drops < 1)
            drops = 1;
        if (drops > 5)
            drops = 5;

        for (int i = 0; i < 5; i++)
        {
            if (i < drops)
            {
                dianui_update_icon(waterDropIcons[i], icon_water_drop, ICON_SIZE);
            }
            else
            {
                dianui_update_icon(waterDropIcons[i], icon_water_drop_empty, ICON_SIZE);
            }
        }

        last_water_level = data.water_level;
    }

    // Update face 
    if (data.pump_state == true)
    {
        dianui_set_emotion(HAPPY);
    }
    else if (data.light <= LDR_NIGHT_THRESHOLD)
    {
        dianui_set_emotion(SLEEPY);
    }
    else if (data.soil_moisture <= MOISTURE_DRY_THRESHOLD)
    {
        dianui_set_emotion(ALMOST_DEAD);
    }
    else if (data.soil_moisture <= MOISTURE_HALF_DRY_THRESHOLD)
    {
        dianui_set_emotion(THIRSTY);
    }
    else
    {
        dianui_set_emotion(IDLE);
    }

    dianui_update_face();

    // Update time
    static char current_time[6];

    time_t now;
    struct tm local_time;

    time(&now);
    localtime_r(&now, &local_time);

    strftime(current_time, sizeof(current_time), "%H:%M", &local_time);
    dianui_update_text(timeText, current_time);
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