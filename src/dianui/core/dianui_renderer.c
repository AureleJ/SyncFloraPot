#include "dianui_renderer.h"
#include <string.h>

static DianUI_HAL *hal = NULL;

void dianui_renderer_init(DianUI_HAL *hal_ptr)
{
    hal = hal_ptr;
}

void dianui_draw_char(char c, int x, int y, DianUI_Color color)
{
    if (c >= DIANUI_MAX_CHAR)
        return;

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
    for (int j = 0; j < h; j++)
        for (int i = 0; i < w; i++)
            hal->set_pixel(x + i, y + j, color);
}

void dianui_draw_border(int x, int y, int w, int h, DianUI_Color color)
{
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
    hal->set_pixel(x, y, color);
}

void dianui_draw_icon(const uint8_t *iconData, int x, int y, int iconSize, DianUI_Color color)
{
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