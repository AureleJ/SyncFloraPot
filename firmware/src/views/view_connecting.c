#include "views.h"
#include "core/system_state.h"

static DianUI_TextElement *connectingText = NULL;
static DianUI_TextElement *statusText = NULL;

static void view_connecting_enter(void)
{
    connectingText = dianui_create_text(0, 20, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "Connecting...", DIANUI_WHITE);
    statusText = dianui_create_text(0, 30, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "Please wait...", DIANUI_WHITE);
}

const DianUI_Page view_connecting = {
    .name = "Connecting",
    .on_enter = view_connecting_enter,
    .on_update = 0,
    .on_input = 0,
    .on_exit = 0,
};