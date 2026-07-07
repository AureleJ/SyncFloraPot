#include "dianui_log.h"
#include "dianui_renderer.h"
#include "dianui_engine_private.h"
#include <string.h>

void dianui_draw_char(char c, int x, int y, DianUI_Color color)
{
    if (c >= DIANUI_MAX_CHAR)
    {
        DIANUI_LOGE("Character is out of bounds for the font. Skipping.");
        return;
    }

    DianUI_HAL *hal = dianui_engine_get_hal();

    const uint8_t *char_bitmap = font[(int)c];

    for (int row = 0; row < DIANUI_FONT_HEIGHT; row++)
    {
        uint8_t line = char_bitmap[row];
        for (int col = 0; col < DIANUI_FONT_WIDTH; col++)
        {
            if (line & (1 << col))
                hal->set_pixel(x + col, y + row, color);
        }
    }
}

void dianui_draw_string(const char *str, int x, int y, DianUI_Color color)
{
    for (int i = 0; str[i] != '\0'; i++)
        dianui_draw_char(str[i], x + (i * DIANUI_FONT_WIDTH), y, color);
}

void dianui_draw_shape(int x, int y, int w, int h, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();

    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            hal->set_pixel(x + i, y + j, color);
}

void dianui_draw_border(int x, int y, int w, int h, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();

    for (int i = 0; i < w; i++)
    {
        hal->set_pixel(x + i, y, color);
        hal->set_pixel(x + i, y + h - 1, color);
    }
    for (int j = 0; j < h; j++)
    {
        hal->set_pixel(x, y + j, color);
        hal->set_pixel(x + w - 1, y + j, color);
    }
}

void dianui_draw_pixel(int x, int y, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();
    hal->set_pixel(x, y, color);
}

void dianui_draw_hline(int x1, int x2, int y, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();
    hal->set_hline(x1, y, x2 - x1 + 1, color);
}

void dianui_draw_line(int x1, int y1, int x2, int y2, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();

    int dx = abs(x2 - x1);
    int dy = abs(y2 - y1);
    int sx = (x1 < x2) ? 1 : -1;
    int sy = (y1 < y2) ? 1 : -1;
    int err = dx - dy;

    while (true)
    {
        hal->set_pixel(x1, y1, color);

        if (x1 == x2 && y1 == y2)
            break;

        int err2 = err * 2;
        if (err2 > -dy)
        {
            err -= dy;
            x1 += sx;
        }
        if (err2 < dx)
        {
            err += dx;
            y1 += sy;
        }
    }
}

void dianui_draw_icon(const uint8_t *iconData, int x, int y, int iconSize, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();
    for (int j = 0; j < iconSize; j++)
    {
        uint8_t line = iconData[j];
        for (int i = 0; i < iconSize; i++)
        {
            if (line & (1 << i))
                hal->set_pixel(x + i, y + j, color);
        }
    }
}