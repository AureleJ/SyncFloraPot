#include "dianui_polygon.h"
#include "../core/dianui_engine_private.h"
#include "../core/dianui_log.h"
#include "../core/dianui_renderer.h"
#include <string.h>
#include <math.h>

static DianUI_PolygonElement polygon_pool[DIANUI_MAX_POLYGONS];
static uint8_t polygon_pool_index = 0;

static void draw_polygon_element(DianUI_BaseElement *self);

DianUI_PolygonElement *dianui_create_polygon(DianUI_Point pos, DianUI_Corner corners[4], DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color)
{
    if (polygon_pool_index >= DIANUI_MAX_POLYGONS)
    {
        return NULL;
    }

    DianUI_PolygonElement *el = &polygon_pool[polygon_pool_index++];
    el->base.x = pos.x;
    el->base.y = pos.y;
    el->base.w = 0;
    el->base.h = 0;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_polygon_element;
    el->base.visible = true;
    el->color = color;

    memcpy(el->corners, corners, sizeof(DianUI_Corner) * 4);

    dianui_engine_register((DianUI_BaseElement *)el);

    return el;
}

static void draw_polygon_element(DianUI_BaseElement *self)
{
    if (!self)
        return;

    DianUI_PolygonElement *el = (DianUI_PolygonElement *)self;

    DianUI_Point START[4];
    DianUI_Point END[4];

    for (int i = 0; i < 4; i++)
    {
        DianUI_Point currentCorner = {.x = el->base.x + el->corners[i].pos.x, .y = el->base.y + el->corners[i].pos.y};
        DianUI_Point nextCorner = {.x = el->base.x + el->corners[(i + 1) % 4].pos.x, .y = el->base.y + el->corners[(i + 1) % 4].pos.y};
        DianUI_Point previousCorner = {.x = el->base.x + el->corners[(i + 3) % 4].pos.x, .y = el->base.y + el->corners[(i + 3) % 4].pos.y};
        int roundness = el->corners[i].roundness;

        float vx_prev = previousCorner.x - currentCorner.x;
        float vy_prev = previousCorner.y - currentCorner.y;
        float len_prev = sqrtf(vx_prev * vx_prev + vy_prev * vy_prev);
        START[i].x = currentCorner.x + (int)roundf((vx_prev / len_prev) * roundness);
        START[i].y = currentCorner.y + (int)roundf((vy_prev / len_prev) * roundness);

        float vx_next = nextCorner.x - currentCorner.x;
        float vy_next = nextCorner.y - currentCorner.y;
        float len_next = sqrtf(vx_next * vx_next + vy_next * vy_next);
        END[i].x = currentCorner.x + (int)roundf((vx_next / len_next) * roundness);
        END[i].y = currentCorner.y + (int)roundf((vy_next / len_next) * roundness);
    }

    int leftWall[64];
    for (int i = 0; i < 64; i++)
    {
        leftWall[i] = 128;
    }

    int rightWall[64];
    for (int i = 0; i < 64; i++)
    {
        rightWall[i] = -128;
    }

    for (int i = 0; i < 4; i++)
    {
        DianUI_Point start = START[i];
        DianUI_Point end = END[i];
        DianUI_Point control = {.x = el->base.x + el->corners[i].pos.x, .y = el->base.y + el->corners[i].pos.y};

        int total_steps = 14;
        for (int step = 1; step <= total_steps; step++)
        {
            float t = (float)step / total_steps;
            DianUI_Point curr_p;
            curr_p.x = (int)roundf((1.0f - t) * (1.0f - t) * start.x + 2.0f * (1.0f - t) * t * control.x + t * t * end.x);
            curr_p.y = (int)roundf((1.0f - t) * (1.0f - t) * start.y + 2.0f * (1.0f - t) * t * control.y + t * t * end.y);

            if (curr_p.x < leftWall[curr_p.y])
            {
                leftWall[curr_p.y] = curr_p.x;
            }
            if (curr_p.x > rightWall[curr_p.y])
            {
                rightWall[curr_p.y] = curr_p.x;
            }
        }

        DianUI_Point next_start = START[(i + 1) % 4];
        // dianui_draw_line(end.x, end.y, next_start.x, next_start.y, el->color);

        /* if (next_start.y == end.y)
        {
            if (next_start.x < end.x)
            {
                leftWall[next_start.y] = next_start.x;
                rightWall[next_start.y] = end.x;
            }
            else
            {
                leftWall[next_start.y] = end.x;
                rightWall[next_start.y] = next_start.x;
            }
            continue;
        } */
        
        int startY;
        int endY;
        int x1, y1, x2, y2;

        if (next_start.y < end.y)
        {
            startY = next_start.y;
            endY = end.y;
            x1 = next_start.x;
            y1 = next_start.y;
            x2 = end.x;
            y2 = end.y;
        }
        else if (next_start.y > end.y)
        {
            startY = end.y;
            endY = next_start.y;
            x1 = end.x;
            y1 = end.y;
            x2 = next_start.x;
            y2 = next_start.y;
        }
        else
        {
            continue; 
        }

        for (int y = startY; y <= endY; y++)
        {
            int x = (int)roundf((float)(y - y1) / (float)(y2 - y1) * (x2 - x1) + x1);

            if (x < leftWall[y])
            {
                leftWall[y] = x;
            }
            if (x > rightWall[y])
            {
                rightWall[y] = x;
            }
        }
    }

    for (int y = 0; y < 64; y++)
    {
        if (leftWall[y] != 128 && rightWall[y] != -128)
        {
            dianui_draw_line(leftWall[y], y, rightWall[y], y, el->color);
        }
    }
}

void dianui_polygons_reset()
{
    polygon_pool_index = 0;
}