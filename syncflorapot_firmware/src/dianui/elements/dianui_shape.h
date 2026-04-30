#ifndef DIANUI_SHAPE_H
#define DIANUI_SHAPE_H

#include "../core/dianui_types.h"

/**
 * @brief Create a new shape element.
 *
 * @param x Initial X position of the element.
 * @param y Initial Y position of the element.
 * @param w Width of the element (can be used for layout).
 * @param h Height of the element (can be used for layout).
 * @param xAnchor Horizontal anchor point for positioning.
 * @param yAnchor Vertical anchor point for positioning.
 * @param color Color of the shape.
 * @return Pointer to the created shape element, or NULL if the shape pool is full.
 */
DianUI_ShapeElement *dianui_create_shape(int x, int y, int w, int h, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color);

/**
 * @brief Reset the shape element pool.
 */
void dianui_shape_reset();

#endif // DIANUI_SHAPE_H