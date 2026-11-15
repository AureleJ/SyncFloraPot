#ifndef DIANUI_RENDERER_H
#define DIANUI_RENDERER_H

#include "dianui_types.h"
#include "dianui_hal.h"
#include "../assets/dianui_font.h"

/**
 * @brief Internal drawing primitives.
 *
 * These functions are used by the engine to draw various UI elements.
 */
void dianui_draw_char(char c, int16_t x, int16_t y, DianUI_Color color);
void dianui_draw_string(const char *str, int16_t x, int16_t y, DianUI_Color color);
void dianui_draw_shape(int16_t x, int16_t y, int16_t w, int16_t h, DianUI_Color color);
void dianui_draw_border(int16_t x, int16_t y, int16_t w, int16_t h, DianUI_Color color);
void dianui_draw_pixel(int16_t x, int16_t y, DianUI_Color color);
void dianui_draw_hline(int16_t x1, int16_t x2, int16_t y, DianUI_Color color);
void dianui_draw_vline(int16_t x, int16_t y1, int16_t y2, DianUI_Color color);
void dianui_draw_icon(const uint8_t *iconData, int16_t x, int16_t y, uint8_t iconSize, DianUI_Color color);
void dianui_draw_line(int16_t x1, int16_t y1, int16_t x2, int16_t y2, DianUI_Color color);

#endif // DIANUI_RENDERER_H