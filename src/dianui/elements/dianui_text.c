#include "dianui_text.h"
#include "../core/dianui_engine_private.h"
#include "../core/dianui_renderer.h"
#include "../core/dianui_config.h"
#include "../core/dianui_log.h"
#include <string.h>

static DianUI_TextElement text_pool[DIANUI_MAX_TEXT];
static int text_pool_index = 0;

static void draw_text_element(DianUI_BaseElement *self);

DianUI_TextElement *dianui_create_text(int x, int y, int w, int h, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, const char *text, DianUI_Color color)
{
    if (text_pool_index >= DIANUI_MAX_TEXT)
    {
        return NULL;
    }

    DianUI_TextElement *el = &text_pool[text_pool_index++];
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

    dianui_engine_register((DianUI_BaseElement *)el);

    return el;
}

void dianui_add_icon_to_text(DianUI_TextElement *textEl, const uint8_t *iconData, int iconSize, int gap)
{
    textEl->iconData = iconData;
    textEl->iconSize = iconSize;
    textEl->iconGap = gap;
    textEl->base.dirty = true;
}

void dianui_update_text(DianUI_TextElement *textEl, const char *newText)
{
    textEl->text = newText;
    textEl->base.dirty = true;
}

static void draw_text_element(DianUI_BaseElement *self)
{
    if (!self)
    {
        DIANUI_LOGE("DianUI element is NULL.");
        return;
    }

    DianUI_TextElement *el = (DianUI_TextElement *)self;

    if (self->border)
    {
        dianui_draw_border(self->x, self->y, self->w, self->h, el->color);
    }

    if (el->color == DIANUI_BLACK)
    {
        dianui_draw_shape(self->x, self->y, self->w, self->h, DIANUI_WHITE);
    }

    int x = self->x;
    int y = self->y;

    int text_height = DIANUI_FONT_HEIGHT;
    int text_width = strlen(el->text) * DIANUI_FONT_WIDTH;

    if (self->xAnchor == DIANUI_CENTER)
    {
        x = self->x + (self->w - text_width) / 2;
    }
    else if (self->xAnchor == DIANUI_RIGHT)
    {
        x = self->x + self->w - text_width;
    }

    if (self->yAnchor == DIANUI_CENTER)
    {
        y = self->y + (self->h - text_height) / 2;
    }
    else if (self->yAnchor == DIANUI_BOTTOM)
    {
        y = self->y + self->h - text_height;
    }

    if (el->iconData != NULL)
    {
        dianui_draw_icon(el->iconData, x, y, el->iconSize, el->color);
        x += el->iconSize + el->iconGap;
    }

    dianui_draw_string(el->text, x, y, el->color);
}

void dianui_text_reset()
{
    text_pool_index = 0;
}