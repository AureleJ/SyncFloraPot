#include "views.h"
#include "esp_system.h"

static void test_callback(void)
{
    return;
}

static void exit_callback(void)
{
    dianui_router_pop();
}

static void about_callback(void)
{
    dianui_router_push(VIEW_ID_ABOUT);
}

static void reboot_callback(void)
{
    esp_restart();
}

static DianUI_Menu *menu;
static float auto_exit_timer = 10.0f;

static void view_settings_enter(void)
{
    auto_exit_timer = 10.0f;
    static DianUI_MenuItem menuItems[] =
        {
            {.label = "Reset Wi-Fi", .callback = test_callback},
            {.label = "Reset Device", .callback = test_callback},
            {.label = "Reboot", .callback = reboot_callback},
            {.label = "Watering Test", .callback = test_callback},
            {.label = "About", .callback = about_callback},
            {.label = "Back", .callback = exit_callback},
        };

    menu = dianui_create_menu("Menu", menuItems, 6);
}

static void view_settings_update(float dt)
{
    dianui_update_menu(menu);

    auto_exit_timer -= dt;

    if (auto_exit_timer <= 0.0f)
    {
        dianui_router_pop();
        auto_exit_timer = 10.0f;
    }
}

static void view_settings_action(DianUI_NavEvent action)
{
    if (action == DIANUI_NAV_SELECT)
    {
        dianui_handle_input(menu, DIANUI_MENU_INPUT_OPEN);
    }
    else if (action == DIANUI_NAV_NEXT)
    {
        auto_exit_timer = 10.0f;
        dianui_handle_input(menu, DIANUI_MENU_INPUT_NEXT);
    }
}

const DianUI_Page view_settings = {
    .name = "Settings",
    .on_enter = view_settings_enter,
    .on_update = view_settings_update,
    .on_input = view_settings_action,
    .on_exit = 0,
};