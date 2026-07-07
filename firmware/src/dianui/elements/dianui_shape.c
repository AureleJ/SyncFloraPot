#include "dianui_shape.h"
#include "../core/dianui_engine_private.h"
#include "../core/dianui_log.h"
#include "../core/dianui_renderer.h"
#include "../core/dianui_config.h"
#include <string.h>

#include "esp_log.h"

static DianUI_RectangleElement rect_pool[DIANUI_MAX_RECTANGLES];
static uint8_t rect_pool_index = 0;

static DianUI_CircleElement circle_pool[DIANUI_MAX_CIRCLES];
static uint8_t circle_pool_index = 0;

// static DianUI_LineElement line_pool[DIANUI_MAX_LINES];
// static uint8_t line_pool_index = 0;

// static DianUI_TriangleElement triangle_pool[DIANUI_MAX_TRIANGLES];
// static uint8_t triangle_pool_index = 0;

static void draw_shape_rectangle_element(DianUI_BaseElement *self);
static void draw_shape_circle_element(DianUI_BaseElement *self);

DianUI_RectangleElement *dianui_create_shape_rectangle(int x, int y, int w, int h, int r, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color)
{
    if (rect_pool_index >= DIANUI_MAX_RECTANGLES)
    {
        return NULL;
    }

    DianUI_RectangleElement *el = &rect_pool[rect_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_shape_rectangle_element;
    el->base.visible = true;
    el->color = color;
    el->radius = r;

    dianui_engine_register((DianUI_BaseElement *)el);

    return el;
}

DianUI_CircleElement *dianui_create_shape_circle(int x, int y, int r, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color)
{
    if (circle_pool_index >= DIANUI_MAX_CIRCLES)
    {
        return NULL;
    }

    DianUI_CircleElement *el = &circle_pool[circle_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = 0;
    el->base.h = 0;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_shape_circle_element;
    el->base.visible = true;
    el->color = color;
    el->radius = r;

    dianui_engine_register((DianUI_BaseElement *)el);

    return el;
}

static void draw_shape_rectangle_element(DianUI_BaseElement *self)
{
    if (!self)
    {
        DIANUI_LOGE("DianUI element is NULL.");
        return;
    }

    DianUI_RectangleElement *el = (DianUI_RectangleElement *)self;

    int radius = ((el->base.w < el->base.h) ? el->base.w / 2 : el->base.h / 2) * el->radius / 100;

    if (el->radius < 0)
    {
        radius = 0;
    }
    else if (el->radius > 100)
    {
        radius = (el->base.w < el->base.h) ? el->base.w / 2 : el->base.h / 2;
    }

    for (int i = radius; i < el->base.w - radius; i++)
    {
        dianui_draw_pixel(el->base.x + i, el->base.y, el->color);
        dianui_draw_pixel(el->base.x + i, el->base.y + el->base.h - 1, el->color);
    }
    for (int j = radius; j < el->base.h - radius; j++)
    {
        dianui_draw_pixel(el->base.x, el->base.y + j, el->color);
        dianui_draw_pixel(el->base.x + el->base.w - 1, el->base.y + j, el->color);
    }

    int circle_x = el->base.x + radius;
    int circle_y = el->base.y + radius;
    int x = 0;
    int y = -radius;
    int d = -radius; 

    while (x < -y)
    {
        if (d > 0)
        {
            y++;
            d += 2 * (x + y) + 1;
        }
        else
        {
            d += 2 * x + 1;
        }

        dianui_draw_pixel(circle_x - x, circle_y + y, el->color); // Top-left-top
        dianui_draw_pixel(circle_x + y, circle_y - x, el->color); // Top-left-bottom

        dianui_draw_pixel(circle_x + x + el->base.w - radius * 2 - 1, circle_y + y, el->color); // Top-right-top
        dianui_draw_pixel(circle_x - y + el->base.w - radius * 2 - 1, circle_y - x, el->color); // Top-right-bottom

        dianui_draw_pixel(circle_x + x + el->base.w - radius * 2 - 1, circle_y - y + el->base.h - radius * 2 - 1, el->color); // Bottom-right-top
        dianui_draw_pixel(circle_x - y + el->base.w - radius * 2 - 1, circle_y + x + el->base.h - radius * 2 - 1, el->color); // Bottom-right-bottom
        
        dianui_draw_pixel(circle_x - x, circle_y - y + el->base.h - radius * 2 - 1, el->color); // Bottom-left-top
        dianui_draw_pixel(circle_x + y, circle_y + x + el->base.h - radius * 2 - 1, el->color); // Bottom-left-bottom

        x++;
    }
}

static void draw_shape_circle_element(DianUI_BaseElement *self)
{
    if (!self)
    {
        DIANUI_LOGE("DianUI element is NULL.");
        return;
    }

    DianUI_CircleElement *el = (DianUI_CircleElement *)self;

    // Mid point circle algorithm
    int x = 0;
    int y = -el->radius;
    int d = -el->radius; 

    while (x < -y)
    {
        if (d > 0)
        {
            y++;
            d += 2 * (x + y) + 1;
        }
        else
        {
            d += 2 * x + 1;
        }

        dianui_draw_pixel(el->base.x + x, el->base.y + y, el->color);
        dianui_draw_pixel(el->base.x - x, el->base.y + y, el->color);
        dianui_draw_pixel(el->base.x + x, el->base.y - y, el->color);
        dianui_draw_pixel(el->base.x - x, el->base.y - y, el->color);
        dianui_draw_pixel(el->base.x + y, el->base.y + x, el->color);
        dianui_draw_pixel(el->base.x - y, el->base.y + x, el->color);
        dianui_draw_pixel(el->base.x + y, el->base.y - x, el->color);
        dianui_draw_pixel(el->base.x - y, el->base.y - x, el->color);

        x++;
    }
}

void dianui_shape_reset()
{
    rect_pool_index = 0;
    circle_pool_index = 0;
    // line_pool_index = 0;
    // triangle_pool_index = 0;
}