#ifndef DIANUI_QR_CODE_H
#define DIANUI_QR_CODE_H

#include "../core/dianui_types.h"

/**
 * @brief Create a new QR code element.
 *
 * @param x Initial X position of the element.
 * @param y Initial Y position of the element.
 * @param w Width of the element (can be used for layout).
 * @param h Height of the element (can be used for layout).
 * @param xAnchor Horizontal anchor point for positioning.
 * @param yAnchor Vertical anchor point for positioning.
 * @param data String to encode in the QR code.
 * @return Pointer to the created QR code element, or NULL if the QR code pool is full.
 */
DianUI_QRCodeElement *dianui_create_qr_code(int x, int y, int w, int h, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, const char *data);

#endif // DIANUI_QR_CODE_H