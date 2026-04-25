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
void dianui_draw_char(char c, int x, int y, DianUI_Color color);
void dianui_draw_string(const char *str, int x, int y, DianUI_Color color);
void dianui_draw_shape(int x, int y, int w, int h, DianUI_Color color);
void dianui_draw_border(int x, int y, int w, int h, DianUI_Color color);
void dianui_draw_pixel(int x, int y, DianUI_Color color);
void dianui_draw_icon(const uint8_t *iconData, int x, int y, int iconSize, DianUI_Color color);

#endif // DIANUI_RENDERER_H