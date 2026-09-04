#include "views.h"

DianUI_TextElement *loadingTimer = 0;

static void view_loading_enter(void)
{
    DianUI_TextElement *loadingText = dianui_create_text(0, 25, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "SyncFloraPot", DIANUI_WHITE);
    DianUI_TextElement *loadingText2 = dianui_create_text(0, 35, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "Loading...", DIANUI_WHITE);
    loadingTimer = dianui_create_text(0, 45, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "5.0s", DIANUI_WHITE);
}

static float loading_timer = 5.0f;

static char timer_buf[16];

static void view_loading_update(float dt)
{
    loading_timer -= dt;

    snprintf(timer_buf, sizeof(timer_buf), "%.1fs", loading_timer);
    dianui_update_text(loadingTimer, timer_buf);

    if (loading_timer <= 0.0f)
    {
        loading_timer = 0.0f;
        dianui_router_push(VIEW_ID_PROVISIONNING);
    }
}

const DianUI_Page view_loading = {
    .name = "Loading",
    .on_enter = view_loading_enter,
    .on_update = view_loading_update,
    .on_input = 0,
    .on_exit = 0,
};