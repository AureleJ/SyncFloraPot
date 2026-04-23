#include "element.h"
#include "icon.h"
#include "../core/engine_private.h"
#include "../core/renderer.h"
#include <string.h>

#define MAX_ICON_ELEMENTS 5

static IconElement icon_pool[MAX_ICON_ELEMENTS];
static int icon_pool_index = 0;

static void draw_icon_element(BaseElement *self);

IconElement *ui_create_icon(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const uint8_t *iconData)
{
    if (icon_pool_index >= MAX_ICON_ELEMENTS)
    {
        return NULL;
    }

    IconElement *el = &icon_pool[icon_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_icon_element;
    el->base.visible = true;
    el->iconData = iconData;

    engine_register_element((BaseElement *)el);

    return el;
}

static void draw_icon_element(BaseElement *self)
{
    IconElement *el = (IconElement *)self;

    for (int y = 0; y < 8; y++)
    {
        uint8_t line = el->iconData[y];
        for (int x = 0; x < 8; x++)
        {
            if (line & (1 << x))
            {
                ui_draw_pixel(self->x + x, self->y + y, WHITE);
            }
        }
    }
}