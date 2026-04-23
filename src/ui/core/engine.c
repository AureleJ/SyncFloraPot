#include "engine.h"
#include "engine_private.h"
#include "renderer.h"
#include <string.h>
#include <stdint.h>

static UI_HAL *hal = NULL;

#define MAX_UI_ELEMENTS 10

static BaseElement *display_registry[MAX_UI_ELEMENTS];
static int element_count = 0;

void ui_init(UI_HAL *hal_ptr)
{
    hal = hal_ptr;
    renderer_init(hal);
}

void engine_register_element(BaseElement *el)
{
    if (element_count >= MAX_UI_ELEMENTS)
    {
        return;
    }

    display_registry[element_count++] = el;
}

void ui_render()
{
    bool needs_redraw = false;

    for (int i = 0; i < element_count; i++)
    {
        if (display_registry[i]->dirty)
        {
            needs_redraw = true;
            break;
        }
    }

    if (!needs_redraw)
        return;

    hal->clear();

    for (int i = 0; i < element_count; i++)
    {
        if (display_registry[i]->render && display_registry[i]->visible)
        {
            display_registry[i]->render(display_registry[i]);
            display_registry[i]->dirty = false;
        }
    }

    hal->update();
}

void ui_clear()
{
    hal->clear();
    hal->update();
}