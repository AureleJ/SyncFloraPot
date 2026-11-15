#ifndef DIANUI_SHAPE_H
#define DIANUI_SHAPE_H

#include "../core/dianui_types.h"

DianUI_RectangleElement *dianui_create_shape_rectangle(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t r, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color);
DianUI_CircleElement *dianui_create_shape_circle(int16_t x, int16_t y, uint8_t r, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color);

/**
 * @brief Reset the shape element pool.
 */
void dianui_shape_reset();

#endif // DIANUI_SHAPE_H