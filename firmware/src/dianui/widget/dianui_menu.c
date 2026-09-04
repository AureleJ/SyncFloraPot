#include "dianui_menu.h"
#include "../core/dianui_config.h"
#include "../core/dianui_renderer.h"
#include "../core/dianui_engine_private.h"
#include "../elements/dianui_shape.h"
#include "../core/dianui_time.h"
#include "../core/dianui_log.h"         
#include "../elements/dianui_text.h"    
#include "math.h"

static DianUI_Menu menu_pool[DIANUI_MAX_MENUS];
static int menu_pool_index = 0;

#define DIANUI_ITEM_HEIGHT 15
#define DIANUI_ITEM_WIDTH 128
#define DIANUI_ITEM_SPACING 2
#define DIANUI_ITEM_SELECTED_SHIFT 7
#define DIANUI_MENU_SHIFT 12

DianUI_Menu *dianui_create_menu(const char *title, DianUI_MenuItem *items, int item_count)
{
    if (item_count <= 0)
    {
        DIANUI_LOGE("Invalid item count for menu. Must be greater than 0.");
        return 0;
    }

    if (menu_pool_index >= DIANUI_MAX_MENUS)
    {
        DIANUI_LOGE("DianUI menu pool is full. Cannot create more menus.");
        return 0;
    }

    DianUI_Menu *menu = &menu_pool[menu_pool_index++];
    menu->title = title;
    menu->items = items;
    menu->item_count = item_count;
    menu->selected_index = 0;
    menu->prev_selected_index = -1;
    menu->linear_progress = 0.0f;

    for (int i = 0; i < menu->item_count; i++)
    {
        int x = dianui_get_screen_width() / 2 - DIANUI_ITEM_WIDTH / 2;
        int y = (dianui_get_screen_height() / 2) - (DIANUI_ITEM_HEIGHT / 2) + i * (DIANUI_ITEM_HEIGHT + DIANUI_ITEM_SPACING) + DIANUI_MENU_SHIFT;

        if (i == 0)
        {
            x = x + DIANUI_ITEM_SELECTED_SHIFT;
        }

        DianUI_TextElement *el = dianui_create_text(x, y, 128, DIANUI_ITEM_HEIGHT, DIANUI_LEFT, DIANUI_CENTER, items[i].label, DIANUI_WHITE);

        items[i].element = el;
    }

    // Header
    dianui_create_shape_rectangle(0, 0, dianui_get_screen_width(), 12, 0, DIANUI_CENTER, DIANUI_CENTER, DIANUI_BLACK); // Background
    dianui_create_shape_rectangle(0, 12, dianui_get_screen_width(), 1, 0, DIANUI_CENTER, DIANUI_CENTER, DIANUI_WHITE); // Bottom line
    dianui_create_text(0, 0, dianui_get_screen_width(), 10, DIANUI_CENTER, DIANUI_CENTER, title, DIANUI_WHITE);        // Title

    // Selector

    // Scrollbar
    DianUI_RectangleElement *scroll_container = dianui_create_shape_rectangle(dianui_get_screen_width() - 5, 16, 5, dianui_get_screen_height() - 16, 10, DIANUI_CENTER, DIANUI_CENTER, DIANUI_BLACK); // Container
    dianui_shape_set_border((DianUI_BaseElement *)scroll_container, true);

    menu->scrollbar_indicator = dianui_create_shape_rectangle(dianui_get_screen_width() - 5, 16, 5, (dianui_get_screen_height() - 16) / menu->item_count, 10, DIANUI_CENTER, DIANUI_CENTER, DIANUI_WHITE); // Indicator

    return menu;
}

void dianui_handle_input(DianUI_Menu *menu, DianUI_MenuInput input)
{
    switch (input)
    {
    case DIANUI_MENU_INPUT_OPEN:
        if (menu->items[menu->selected_index].callback)
            menu->items[menu->selected_index].callback();
        break;
    case DIANUI_MENU_INPUT_PREV:
        menu->selected_index = (menu->selected_index - 1 + menu->item_count) % menu->item_count;
        break;
    case DIANUI_MENU_INPUT_NEXT:
        menu->selected_index = (menu->selected_index + 1) % menu->item_count;
        break;
    default:
        break;
    }
}

void dianui_update_menu(DianUI_Menu *menu)
{
    if (!menu)
        return;

    const float ANIMATION_SPEED = 1.0f / 0.3f;

    if (menu->prev_selected_index != menu->selected_index)
    {
        menu->prev_selected_index = menu->selected_index;
        menu->linear_progress = 0.0f;

        for (int i = 0; i < menu->item_count; i++)
        {
            menu->items[i].start_x = menu->items[i].element->base.x;
            menu->items[i].start_y = menu->items[i].element->base.y;
        }

        // menu->scrollbar_indicator_start_y = menu->scrollbar_indicator->base.y;
    }

    menu->linear_progress += ANIMATION_SPEED * dianui_time_get_dt();

    if (menu->linear_progress >= 1.0f)
    {
        menu->linear_progress = 1.0f;
    }

    float invT = 1.0f - menu->linear_progress;
    float t_eased = 1.0f - invT * invT;

    for (int i = 0; i < menu->item_count; i++)
    {
        int targetY = (dianui_get_screen_height() / 2) - (DIANUI_ITEM_HEIGHT / 2) + (i - menu->selected_index) * (DIANUI_ITEM_HEIGHT + DIANUI_ITEM_SPACING) + DIANUI_MENU_SHIFT;

        int targetX = dianui_get_screen_width() / 2 - DIANUI_ITEM_WIDTH / 2;

        if (i == menu->selected_index)
        {
            targetX += DIANUI_ITEM_SELECTED_SHIFT;
        }

        float current_y = (float)menu->items[i].start_y + ((float)targetY - (float)menu->items[i].start_y) * t_eased;
        float current_x = (float)menu->items[i].start_x + ((float)targetX - (float)menu->items[i].start_x) * t_eased;

        menu->items[i].element->base.y = (int)(current_y + 0.5f);
        menu->items[i].element->base.x = (int)(current_x + 0.5f);
        menu->items[i].element->base.dirty = true;
    }

    int selectorToY = 16 + (dianui_get_screen_height() - 16) * menu->selected_index / menu->item_count;
    menu->scrollbar_indicator->base.y = (int)(menu->scrollbar_indicator->base.y * (1 - t_eased) + selectorToY * t_eased);
    menu->scrollbar_indicator->base.dirty = true;
}

void dianui_menu_reset()
{
    menu_pool_index = 0;
}