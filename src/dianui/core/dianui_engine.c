#include "dianui_engine.h"
#include "dianui_engine_private.h"
#include "dianui_renderer.h"
#include "dianui_config.h"
#include <string.h>
#include <stdint.h>

static DianUI_HAL *hal = NULL;

static DianUI_BaseElement *registry[DIANUI_MAX_ELEMENTS];
static int element_count = 0;

void dianui_init(DianUI_HAL *hal_ptr)
{
    hal = hal_ptr;
    dianui_renderer_init(hal);
}

void dianui_engine_register(DianUI_BaseElement *el)
{
    if (element_count >= DIANUI_MAX_ELEMENTS)
        return;

    registry[element_count++] = el;
}

void dianui_render(void)
{
    bool needs_redraw = false;

    for (int i = 0; i < element_count; i++)
    {
        if (registry[i]->dirty)
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
        if (registry[i]->render && registry[i]->visible)
        {
            registry[i]->render(registry[i]);
            registry[i]->dirty = false;
        }
    }

    hal->update();
}

void dianui_clear(void)
{
    hal->clear();
    hal->update();

    /* Mark all elements dirty so next render redraws everything */
    for (int i = 0; i < element_count; i++)
        registry[i]->dirty = true;
}