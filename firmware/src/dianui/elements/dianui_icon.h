#ifndef DIANUI_ICON_H
#define DIANUI_ICON_H

#include "../core/dianui_types.h"

/**
 * @brief Create a new icon element.
 *
 * @param x Initial X position of the element.
 * @param y Initial Y position of the element.
 * @param w Width of the element (can be used for layout).
 * @param h Height of the element (can be used for layout).
 * @param xAnchor Horizontal anchor point for positioning.
 * @param yAnchor Vertical anchor point for positioning.
 * @param iconData Pointer to bitmap data for the icon (1 byte per pixel).
 * @param iconSize Size of the icon in pixels (assumes square icons).
 * @return Pointer to the created icon element, or NULL if the icon pool is full.
 */
DianUI_IconElement *dianui_create_icon(int16_t x, int16_t y, int16_t w, int16_t h, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, const uint8_t *iconData, uint8_t iconSize);

DianUI_IconElement *dianui_update_icon(DianUI_IconElement *icon, const uint8_t *iconData, uint8_t iconSize);

/**
 * @brief Reset the icon element pool.
 */
void dianui_icon_reset();

#endif // DIANUI_ICON_H