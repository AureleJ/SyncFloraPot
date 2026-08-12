#include "dianui_menu.h"
#include "../core/dianui_config.h"
#include "../core/dianui_renderer.h"
#include "../core/dianui_engine_private.h"
#include "../elements/dianui_shape.h"
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
    if (menu_pool_index >= DIANUI_MAX_MENUS)
    {
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

        if (y < 0)
        {
            continue;
        }

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

    menu->selector = dianui_create_text(dianui_get_screen_width() - 10, 0, 8, DIANUI_ITEM_HEIGHT, DIANUI_LEFT, DIANUI_CENTER, ">", DIANUI_WHITE);

    return menu;
}

void dianui_handle_input(DianUI_Menu *menu, DianUI_MenuInput input)
{
    switch (input)
    {
    case DIANUI_MENU_INPUT_OPEN:
        break;
    case DIANUI_MENU_INPUT_PREV:
        menu->selected_index = (menu->selected_index - 1 + menu->item_count) % menu->item_count;
        break;
    case DIANUI_MENU_INPUT_NEXT:
        menu->selected_index = (menu->selected_index + 1) % menu->item_count;
    }
}

void dianui_update_menu(DianUI_Menu *menu, uint32_t now)
{
    menu->linear_progress += 0.1;

    if (menu->prev_selected_index != menu->selected_index)
    {
        menu->prev_selected_index = menu->selected_index;
        menu->linear_progress = 0.0f;
    }

    if (menu->linear_progress > 1.0f)
    {
        menu->linear_progress = 1.0f;
    }

    float invT = 1.0f - menu->linear_progress;
    float t_eased = 1.0f - invT * invT;

    for (int i = 0; i < menu->item_count; i++)
    {
        int toY = (dianui_get_screen_height() / 2) - (DIANUI_ITEM_HEIGHT / 2) + (i - menu->selected_index) * (DIANUI_ITEM_HEIGHT + DIANUI_ITEM_SPACING) + DIANUI_MENU_SHIFT;
        menu->items[i].element->base.y = (int)(menu->items[i].element->base.y * (1 - t_eased) + toY * t_eased);

        if (i == menu->selected_index)
        {
            int toX = dianui_get_screen_width() / 2 - DIANUI_ITEM_WIDTH / 2 + DIANUI_ITEM_SELECTED_SHIFT;
            menu->items[i].element->base.x = (int)(menu->items[i].element->base.x * (1 - t_eased) + toX * t_eased);
        }
        else
        {
            int toX = dianui_get_screen_width() / 2 - DIANUI_ITEM_WIDTH / 2;
            menu->items[i].element->base.x = (int)(menu->items[i].element->base.x * (1 - t_eased) + toX * t_eased);
        }

        menu->items[i].element->base.dirty = true;
    }

    if (menu->selected_index == menu->item_count - 1)
    {
        dianui_update_text(menu->selector, "<");
    }
    else
    {
        dianui_update_text(menu->selector, ">");
    }

    menu->selector->base.y = menu->items[menu->selected_index].element->base.y;
    menu->selector->base.dirty = true;
}