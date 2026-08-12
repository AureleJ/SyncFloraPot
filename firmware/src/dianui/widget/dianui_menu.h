#ifndef DIANUI_MENU_H
#define DIANUI_MENU_H

#include "dianui.h"

/* typedef enum
{
    DIANUI_MENU_ACTION_CALLBACK,
    DIANUI_MENU_ACTION_SUBMENU,
    DIANUI_MENU_ACTION_OPEN_PAGE
} DianUI_MenuActionType; */

typedef enum {
    DIANUI_MENU_INPUT_PREV,
    DIANUI_MENU_INPUT_NEXT,
    DIANUI_MENU_INPUT_OPEN
} DianUI_MenuInput;

typedef struct
{
    const char *label;
    DianUI_TextElement *element;
    // DianUI_MenuActionType action_type;
    /* union
    {
        void (*callback)(void);
        DianUI_Menu *submenu;
        DianUI_Page *page;
    } action; */
} DianUI_MenuItem;

typedef struct DianUI_Menu
{
    const char *title;
    DianUI_MenuItem *items;
    DianUI_TextElement *selector;
    int item_count;
    int selected_index;
    int prev_selected_index;
    float linear_progress;
} DianUI_Menu;

DianUI_Menu *dianui_create_menu(const char *title, DianUI_MenuItem *items, int item_count);
void dianui_handle_input(DianUI_Menu *menu, DianUI_MenuInput input);
void dianui_update_menu(DianUI_Menu *menu, uint32_t now);

#endif // DIANUI_MENU_H