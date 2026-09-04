#ifndef DIANUI_MENU_H
#define DIANUI_MENU_H

#include "../core/dianui_types.h"

typedef enum
{
    DIANUI_MENU_INPUT_PREV,
    DIANUI_MENU_INPUT_NEXT,
    DIANUI_MENU_INPUT_OPEN
} DianUI_MenuInput;

typedef struct
{
    const char *label;
    DianUI_TextElement *element;
    void (*callback)(void);
    int start_x;
    int start_y;
} DianUI_MenuItem;

typedef struct DianUI_Menu
{
    const char *title;
    DianUI_MenuItem *items;
    DianUI_TextElement *selector;
    DianUI_RectangleElement *scrollbar_indicator;
    int item_count;
    int selected_index;
    int prev_selected_index;
    float linear_progress;
    uint32_t last_time;
} DianUI_Menu;

DianUI_Menu *dianui_create_menu(const char *title, DianUI_MenuItem *items, int item_count);
void dianui_handle_input(DianUI_Menu *menu, DianUI_MenuInput input);
void dianui_update_menu(DianUI_Menu *menu);
void dianui_menu_reset();

#endif // DIANUI_MENU_H