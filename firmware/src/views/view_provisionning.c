#include "views.h"
#include "core/system_state.h"
#include "network/wifi_manager.h"

static DianUI_TextElement *provDeviceName = NULL;
static DianUI_TextElement *provPinCode = NULL;

static void view_provisionning_enter(void)
{
    provDeviceName = dianui_create_text(0, 20, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "Device Name", DIANUI_WHITE);
    provPinCode = dianui_create_text(0, 30, 128, 8, DIANUI_CENTER, DIANUI_CENTER, "PIN Code", DIANUI_WHITE);
}

system_state_t last_state = -1;
static char deviceNameBuf[32];
static char pinCodeBuf[32];

static void view_provisionning_update(float dt)
{
    system_state_t current_state = system_state_get();
        
    if (current_state != last_state)
        {
            last_state = current_state;

            switch (current_state)
            {
            case SYS_STATE_PROVISIONING:
                provisioning_info_t info = network_state_get_device_info();

                if (info.device_name[0] != '\0' && info.pin_code[0] != '\0')
                {
                    snprintf(deviceNameBuf, sizeof(deviceNameBuf), "%s", info.device_name);
                    snprintf(pinCodeBuf, sizeof(pinCodeBuf), "%s", info.pin_code);

                    dianui_update_text(provDeviceName, deviceNameBuf);
                    dianui_update_text(provPinCode, pinCodeBuf);
                }
                break;

            case SYS_STATE_CONNECTED:
                dianui_router_push(VIEW_ID_HOME);
                break;

            case SYS_STATE_ERROR:
                dianui_router_push(VIEW_ID_ERROR);
                break;

            case SYS_STATE_CONNECTING:
                dianui_router_push(VIEW_ID_LOADING);
                break;

            default:
                break;
            }
        } 
}

static void view_provisionning_action(DianUI_NavEvent action)
{
}

const DianUI_Page view_provisionning = {
    .name = "Provisionning",
    .on_enter = view_provisionning_enter,
    .on_update = view_provisionning_update,
    .on_input = view_provisionning_action,
    .on_exit = 0,
};