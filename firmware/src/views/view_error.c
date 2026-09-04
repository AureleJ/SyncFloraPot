#include "views.h"
#include "core/system_state.h"
#include "network/wifi_manager.h"

static DianUI_TextElement *errorText = NULL;

static void view_error_enter(void)
{
    errorText = dianui_create_text(0, 20, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "Error", DIANUI_WHITE);
}

const DianUI_Page view_error = {
    .name = "Error",
    .on_enter = view_error_enter,
    .on_update = 0,
    .on_input = 0,
    .on_exit = 0,
};