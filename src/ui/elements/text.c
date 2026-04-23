#include "element.h"
#include "text.h"
#include "../core/engine_private.h"
#include "../core/renderer.h"
#include <string.h>

#define MAX_TEXT_ELEMENTS 10

static TextElement text_pool[MAX_TEXT_ELEMENTS];
static int text_pool_index = 0;

static void draw_text_element(BaseElement *self);

TextElement *ui_create_text(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *text, Color color)
{
    if (text_pool_index >= MAX_TEXT_ELEMENTS)
    {
        return NULL;
    }

    TextElement *el = &text_pool[text_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_text_element;
    el->base.visible = true;
    el->text = text;
    el->color = color;

    engine_register_element((BaseElement *)el);

    return el;
}

void ui_add_icon_to_text(TextElement *textEl, const uint8_t *iconData, int gap)
{
    textEl->iconData = iconData;
    textEl->iconGap = gap;
    textEl->base.dirty = true;
}

static void draw_text_element(BaseElement *self)
{
    TextElement *el = (TextElement *)self;

    if (self->border)
    {
        ui_draw_border(self->x, self->y, self->w, self->h, el->color);
    }

    if (el->color == BLACK)
    {
        ui_draw_shape(self->x, self->y, self->w, self->h, WHITE);
    }

    int x = self->x;
    int y = self->y;

    int text_height = 8;
    int text_width = strlen(el->text) * 8;

    if (self->xAnchor == CENTER)
    {
        x = self->x + (self->w - text_width) / 2;
    }
    else if (self->xAnchor == RIGHT)
    {
        x = self->x + self->w - text_width;
    }

    if (self->yAnchor == CENTER)
    {
        y = self->y + (self->h - text_height) / 2;
    }
    else if (self->yAnchor == BOTTOM)
    {
        y = self->y + self->h - text_height;
    }

    if (el->iconData != NULL)
    {
        for (int i = 0; i < 8; i++)
        {
            uint8_t line = el->iconData[i];
            for (int j = 0; j < 8; j++)
            {
                if (line & (1 << j))
                {
                    ui_draw_pixel(x + j, y + i, true);
                }
            }
        }
        x += 8 + el->iconGap;
    }

    ui_draw_string(el->text, x, y, el->color);
}