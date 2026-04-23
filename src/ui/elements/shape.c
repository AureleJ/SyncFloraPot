#include "element.h"
#include "shape.h"
#include "../core/engine_private.h"
#include "../core/renderer.h"
#include <string.h>

#define MAX_SHAPE_ELEMENTS 10

static ShapeElement shape_pool[MAX_SHAPE_ELEMENTS];
static int shape_pool_index = 0;

static void draw_shape_element(BaseElement *self);

ShapeElement *ui_create_shape(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, Color color)
{
    if (shape_pool_index >= MAX_SHAPE_ELEMENTS)
    {
        return NULL;
    }

    ShapeElement *el = &shape_pool[shape_pool_index++];
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

    engine_register_element((BaseElement *)el);

    return el;
}

static void draw_shape_element(BaseElement *self)
{
    ShapeElement *el = (ShapeElement *)self;

    ui_draw_shape(self->x, self->y, self->w, self->h, el->color);

    if (self->border)
    {
        ui_draw_border(self->x, self->y, self->w, self->h, el->color);
    }
}