#ifndef DIANUI_TEXT_H
#define DIANUI_TEXT_H

#include "../core/dianui_types.h"

/** @brief Create a new text element.
 *
 * @param x Initial X position of the element.
 * @param y Initial Y position of the element.
 * @param w Width of the element (can be used for layout).
 * @param h Height of the element (can be used for layout).
 * @param xAnchor Horizontal anchor point for positioning.
 * @param yAnchor Vertical anchor point for positioning.
 * @param text Pointer to the string to display (must remain valid while element exists).
 * @param color Text color.
 * @return Pointer to the created text element, or NULL if the text pool is full.
 */
DianUI_TextElement *dianui_create_text(int x, int y, int w, int h, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, const char *text, DianUI_Color color);

/**
 * @brief Add an icon to a text element (displayed before text).
 *
 * @param textEl Pointer to the text element.
 * @param iconData Pointer to bitmap data for the icon (1 byte per pixel).
 * @param iconSize Size of the icon in pixels (assumes square icons).
 * @param gap Gap between the icon and the text.
 * @return Pointer to the created text element, or NULL if the text pool is full.
 */
void dianui_add_icon_to_text(DianUI_TextElement *textEl, const uint8_t *iconData, int iconSize, int gap);

/**
 * @brief Update the text of a text element.
 *
 * @param textEl Pointer to the text element.
 * @param newText Pointer to the new string to display (must remain valid while element exists).
 */
void dianui_update_text(DianUI_TextElement *textEl, const char *newText);

/**
 * @brief Reset the text element pool.
 */
void dianui_text_reset();

#endif // DIANUI_TEXT_H