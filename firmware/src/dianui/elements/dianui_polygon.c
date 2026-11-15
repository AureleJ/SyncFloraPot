#include "dianui_polygon.h"
#include "../core/dianui_engine_private.h"
#include "../core/dianui_log.h"
#include "../core/dianui_renderer.h"
#include <string.h>
#include <math.h>

static DianUI_PolygonElement polygon_pool[DIANUI_MAX_POLYGONS];
static uint8_t polygon_pool_index = 0;

static void draw_polygon_element(DianUI_BaseElement *self);

DianUI_PolygonElement *dianui_create_polygon(DianUI_Point pos, const DianUI_Corner corners[4], DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color)
{
    if (polygon_pool_index >= DIANUI_MAX_POLYGONS)
    {
        return NULL;
    }

    // Bounding box computation
    int16_t minX = corners[0].pos.x;
    int16_t maxX = corners[0].pos.x;
    int16_t minY = corners[0].pos.y;
    int16_t maxY = corners[0].pos.y;

    for (uint8_t i = 1; i < 4; i++)
    {
        if (corners[i].pos.x < minX)
            minX = corners[i].pos.x;
        if (corners[i].pos.x > maxX)
            maxX = corners[i].pos.x;
        if (corners[i].pos.y < minY)
            minY = corners[i].pos.y;
        if (corners[i].pos.y > maxY)
            maxY = corners[i].pos.y;
    }

    DianUI_PolygonElement *el = &polygon_pool[polygon_pool_index++];
    el->base.x = pos.x;
    el->base.y = pos.y;
    el->base.h = maxY - minY;
    el->base.w = maxX - minX;
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
    int16_t screenW = (int16_t)dianui_get_screen_width();
    int16_t screenH = (int16_t)dianui_get_screen_height();

    // Anchor on the center of the polygon
    int16_t minX = el->corners[0].pos.x;
    int16_t maxX = el->corners[0].pos.x;
    int16_t minY = el->corners[0].pos.y;
    int16_t maxY = el->corners[0].pos.y;

    for (uint8_t i = 1; i < 4; i++)
    {
        if (el->corners[i].pos.x < minX) minX = el->corners[i].pos.x;
        if (el->corners[i].pos.x > maxX) maxX = el->corners[i].pos.x;
        if (el->corners[i].pos.y < minY) minY = el->corners[i].pos.y;
        if (el->corners[i].pos.y > maxY) maxY = el->corners[i].pos.y;
    }

    el->base.w = maxX - minX;
    el->base.h = maxY - minY;

    int16_t posX = el->base.x - (el->base.w / 2);
    int16_t posY = el->base.y - (el->base.h / 2);

    // Bezier anchor points for each corner
    DianUI_Point START[4];
    DianUI_Point END[4];

    for (uint8_t i = 0; i < 4; i++)
    {
        DianUI_Point currentCorner = {.x = posX + el->corners[i].pos.x, .y = posY + el->corners[i].pos.y};
        DianUI_Point nextCorner = {.x = posX + el->corners[(i + 1) % 4].pos.x, .y = posY + el->corners[(i + 1) % 4].pos.y};
        DianUI_Point previousCorner = {.x = posX + el->corners[(i + 3) % 4].pos.x, .y = posY + el->corners[(i + 3) % 4].pos.y};
        uint8_t roundness = el->corners[i].roundness;

        /* Vector normalization needs real fractional math (sqrt), kept as float on purpose. */
        float vx_prev = previousCorner.x - currentCorner.x;
        float vy_prev = previousCorner.y - currentCorner.y;
        float len_prev = sqrtf(vx_prev * vx_prev + vy_prev * vy_prev);
        if (len_prev <= 0.0f)
            len_prev = 1.0f;
        START[i].x = currentCorner.x + (int16_t)roundf((vx_prev / len_prev) * roundness);
        START[i].y = currentCorner.y + (int16_t)roundf((vy_prev / len_prev) * roundness);

        float vx_next = nextCorner.x - currentCorner.x;
        float vy_next = nextCorner.y - currentCorner.y;
        float len_next = sqrtf(vx_next * vx_next + vy_next * vy_next);
        if (len_next <= 0.0f)
            len_next = 1.0f;
        END[i].x = currentCorner.x + (int16_t)roundf((vx_next / len_next) * roundness);
        END[i].y = currentCorner.y + (int16_t)roundf((vy_next / len_next) * roundness);
    }

    // Span Buffer
    int16_t leftWall[64];
    int16_t rightWall[64];
    for (int16_t i = 0; i < screenH; i++)
    {
        leftWall[i] = 1000;
        rightWall[i] = -1000;
    }

    // Rasterization
    for (uint8_t i = 0; i < 4; i++)
    {
        DianUI_Point start = START[i];
        DianUI_Point end = END[i];
        DianUI_Point control = {.x = posX + el->corners[i].pos.x, .y = posY + el->corners[i].pos.y};

        // Bezier curve rasterization
        const uint8_t total_steps = 14;
        for (uint8_t step = 1; step <= total_steps; step++)
        {
            /* t is a 0..1 progress fraction along the curve, kept as float like easing. */
            float t = (float)step / total_steps;
            DianUI_Point curr_p;
            curr_p.x = (int16_t)roundf((1.0f - t) * (1.0f - t) * start.x + 2.0f * (1.0f - t) * t * control.x + t * t * end.x);
            curr_p.y = (int16_t)roundf((1.0f - t) * (1.0f - t) * start.y + 2.0f * (1.0f - t) * t * control.y + t * t * end.y);

            if (curr_p.y >= 0 && curr_p.y < screenH)
            {
                if (curr_p.x < leftWall[curr_p.y])
                {
                    leftWall[curr_p.y] = curr_p.x;
                }
                if (curr_p.x > rightWall[curr_p.y])
                {
                    rightWall[curr_p.y] = curr_p.x;
                }
            }
        }

        // Line segment between corners
        DianUI_Point next_start = START[(i + 1) % 4];

        int16_t startY;
        int16_t endY;
        int16_t x1, y1, x2, y2;

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

        for (int16_t y = startY; y <= endY; y++)
        {
            if (y < 0 || y >= screenH)
                continue;

            int16_t x = (int16_t)roundf((float)(y - y1) / (float)(y2 - y1) * (x2 - x1) + x1);

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

    // Scanline filling
    for (int16_t y = 0; y < screenH; y++)
    {
        if (leftWall[y] <= rightWall[y])
        {
            int16_t x1 = leftWall[y] < 0 ? 0 : leftWall[y];
            int16_t x2 = rightWall[y] >= screenW ? screenW - 1 : rightWall[y];

            if (x1 <= x2)
            {
                dianui_draw_hline(x1, x2, y, el->color);
            }
        }
    }

    if (el->base.border)
    {
        dianui_draw_hline(posX, posX + el->base.w, posY, el->color);              // Top
        dianui_draw_hline(posX, posX + el->base.w, posY + el->base.h, el->color); // Bottom
        dianui_draw_vline(posX, posY, posY + el->base.h, el->color);              // Left
        dianui_draw_vline(posX + el->base.w, posY, posY + el->base.h, el->color); // Right
    }
}

void dianui_polygons_reset()
{
    polygon_pool_index = 0;
}
