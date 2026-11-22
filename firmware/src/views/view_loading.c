#include "views.h"
#include "core/system_state.h"

static DianUI_RectangleElement *loadingBarFill = 0;
static float loading_timer;

static void view_loading_enter(void)
{
    loading_timer = 3.0f;
    dianui_create_text(0, 15, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "SyncFloraPot", DIANUI_WHITE);
    DianUI_RectangleElement *loadingBar = dianui_create_shape_rectangle(0, 30, 128, 6, 10, DIANUI_LEFT, DIANUI_CENTER, DIANUI_BLACK);
    dianui_shape_set_border((DianUI_BaseElement *)loadingBar, true);
    loadingBarFill = dianui_create_shape_rectangle(1, 31, 1, 4, 0, DIANUI_LEFT, DIANUI_CENTER, DIANUI_WHITE);
}

static void view_loading_update(float dt)
{
    loading_timer -= dt;

    if (loading_timer < 0.0f)
    {
        loading_timer = 0.0f;
    }

    system_state_t current_state = system_state_get();

    loadingBarFill->base.w = (int)((1.0f - loading_timer / 3.0f) * 126.0f);
    loadingBarFill->base.dirty = true;

    if (loading_timer <= 0.0f)
    {
        if (current_state == SYS_STATE_PROVISIONING)
        {
            dianui_router_push(VIEW_ID_PROVISIONNING);
        }
        else if (current_state == SYS_STATE_CONNECTING)
        {
            dianui_router_push(VIEW_ID_CONNECTING);
        }
        else if (current_state == SYS_STATE_CONNECTED)
        {
            dianui_router_push(VIEW_ID_HOME);
        }
        else
        {
            dianui_router_push(VIEW_ID_ERROR);
        }
    }
}

const DianUI_Page view_loading = {
    .name = "Loading",
    .on_enter = view_loading_enter,
    .on_update = view_loading_update,
    .on_input = 0,
    .on_exit = 0,
};