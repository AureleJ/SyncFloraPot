#include "dianui_element.h"
#include "dianui_shape.h"
#include "../core/dianui_engine_private.h"
#include "../core/dianui_renderer.h"
#include "../core/dianui_config.h"
#include <string.h>

static DianUI_ShapeElement shape_pool[DIANUI_MAX_SHAPES];
static int shape_pool_index = 0;

static void draw_shape_element(DianUI_BaseElement *self);

DianUI_ShapeElement *dianui_create_shape(int x, int y, int w, int h, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color)
{
    if (shape_pool_index >= DIANUI_MAX_SHAPES)
    {
        return NULL;
    }

    DianUI_ShapeElement *el = &shape_pool[shape_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_shape_element;
    el->base.visible = true;
    el->color = color;

    dianui_engine_register((DianUI_BaseElement *)el);

    return el;
}

static void draw_shape_element(DianUI_BaseElement *self)
{
    DianUI_ShapeElement *el = (DianUI_ShapeElement *)self;

    dianui_draw_shape(self->x, self->y, self->w, self->h, el->color);

    if (self->border)
    {
        dianui_draw_border(self->x, self->y, self->w, self->h, el->color);
    }
}