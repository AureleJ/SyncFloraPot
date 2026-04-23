#include "renderer.h"
#include "../assets/font.h"
#include <string.h>

static UI_HAL *hal = NULL;

void renderer_init(UI_HAL *hal_ptr)
{
    hal = hal_ptr;
}

void ui_draw_char(char c, int x, int y, Color color)
{
    if (c >= MAX_CHAR)
        return;

    const uint8_t *char_bitmap = font[(int)c];

    for (int row = 0; row < FONT_HEIGHT; row++)
    {
        uint8_t line = char_bitmap[row];
        for (int col = 0; col < FONT_WIDTH; col++)
        {
            if (line & (1 << col))
            {
                hal->set_pixel(x + col, y + row, color);
            }
        }
    }
}

void ui_draw_string(const char *str, int x, int y, Color color)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        ui_draw_char(str[i], x + (i * FONT_WIDTH), y, color);
    }
}

void ui_draw_shape(int x, int y, int w, int h, Color color)
{
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            hal->set_pixel(x + i, y + j, color);
        }
    }
}

void ui_draw_border(int x, int y, int w, int h, Color color)
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

void ui_draw_pixel(int x, int y, Color color)
{
    hal->set_pixel(x, y, color);
}