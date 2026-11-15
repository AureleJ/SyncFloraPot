#include "dianui_log.h"
#include "dianui_renderer.h"
#include "dianui_engine_private.h"
#include <string.h>
#include <stdlib.h>

void dianui_draw_char(char c, int16_t x, int16_t y, DianUI_Color color)
{
    uint8_t uc = (uint8_t)c;

    if (uc < DIANUI_FONT_FIRST_CHAR || uc > DIANUI_FONT_LAST_CHAR)
        return;

    DianUI_HAL *hal = dianui_engine_get_hal();
    const uint8_t *char_bitmap = dianui_font_8x8[uc - DIANUI_FONT_FIRST_CHAR];

    for (uint8_t row = 0; row < DIANUI_FONT_HEIGHT; row++)
    {
        uint8_t line = char_bitmap[row];
        for (uint8_t col = 0; col < DIANUI_FONT_WIDTH; col++)
        {
            if (line & (1 << col))
            {
                hal->set_pixel(x + col, y + row, color);
            }
        }
    }
}

void dianui_draw_string(const char *str, int16_t x, int16_t y, DianUI_Color color)
{
    for (int16_t i = 0; str[i] != '\0'; i++)
        dianui_draw_char(str[i], x + (i * DIANUI_FONT_WIDTH), y, color);
}

void dianui_draw_shape(int16_t x, int16_t y, int16_t w, int16_t h, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();

    for (int16_t j = 0; j < h; j++)
        for (int16_t i = 0; i < w; i++)
            hal->set_pixel(x + i, y + j, color);
}

void dianui_draw_border(int16_t x, int16_t y, int16_t w, int16_t h, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();

    for (int16_t i = 0; i < w; i++)
    {
        hal->set_pixel(x + i, y, color);
        hal->set_pixel(x + i, y + h - 1, color);
    }
    for (int16_t j = 0; j < h; j++)
    {
        hal->set_pixel(x, y + j, color);
        hal->set_pixel(x + w - 1, y + j, color);
    }
}

void dianui_draw_pixel(int16_t x, int16_t y, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();
    hal->set_pixel(x, y, color);
}

void dianui_draw_hline(int16_t x1, int16_t x2, int16_t y, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();
    hal->set_hline(x1, x2, y, color);
}

void dianui_draw_vline(int16_t x, int16_t y1, int16_t y2, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();
    hal->set_vline(x, y1, y2, color);
}

void dianui_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();

    int16_t dx = abs(x2 - x1);
    int16_t dy = abs(y2 - y1);
    int8_t sx = (x1 < x2) ? 1 : -1;
    int8_t sy = (y1 < y2) ? 1 : -1;
    int16_t err = dx - dy;

    while (true)
    {
        hal->set_pixel(x1, y1, color);

        if (x1 == x2 && y1 == y2)
            break;

        int16_t err2 = err * 2;
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

void dianui_draw_icon(const uint8_t *iconData, int16_t x, int16_t y, uint8_t iconSize, DianUI_Color color)
{
    DianUI_HAL *hal = dianui_engine_get_hal();
    for (uint8_t j = 0; j < iconSize; j++)
    {
        uint8_t line = iconData[j];
        for (uint8_t i = 0; i < iconSize; i++)
        {
            if (line & (1 << i))
                hal->set_pixel(x + i, y + j, color);
        }
    }
}
