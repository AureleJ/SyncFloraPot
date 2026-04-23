#ifndef UI_RENDERER_H
#define UI_RENDERER_H

#include "ui_types.h"
#include "hal.h"

void renderer_init(UI_HAL *hal_ptr);

void ui_draw_char(char c, int x, int y, Color color);

void ui_draw_string(const char *str, int x, int y, Color color);

void ui_draw_shape(int x, int y, int w, int h, Color color);

void ui_draw_border(int x, int y, int w, int h, Color color);

void ui_draw_pixel(int x, int y, Color color);

#endif // UI_RENDERER_H