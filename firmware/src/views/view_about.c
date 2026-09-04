#include "views.h"
#include "dianui/dianui.h"
#include "dianui/core/dianui_types.h"
#include "dianui/pages/dianui_pages.h"

static void view_about_enter(void)
{
    dianui_create_text(0, 10, 128, 8, DIANUI_CENTER, DIANUI_TOP, "SyncFloraPot", DIANUI_WHITE);
    dianui_create_text(0, 20, 128, 8, DIANUI_CENTER, DIANUI_TOP, "Firmware v1.0", DIANUI_WHITE);
    dianui_create_text(0, 30, 128, 8, DIANUI_CENTER, DIANUI_TOP, "Aurele Joblet", DIANUI_WHITE);
    dianui_create_text(0, 40, 128, 8, DIANUI_CENTER, DIANUI_TOP, "aurelejoblet.com", DIANUI_WHITE);
}

static void view_about_action(DianUI_NavEvent action)
{
    if (action == DIANUI_NAV_SELECT || action == DIANUI_NAV_NEXT)
    {
        dianui_router_pop();
    }
}

static void view_about_update(float dt)
{
}

const DianUI_Page view_about = {
    .name = "About",
    .on_enter = view_about_enter,
    .on_update = view_about_update,
    .on_input = view_about_action,
    .on_exit = 0,
};