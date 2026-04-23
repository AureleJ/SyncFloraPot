#ifndef ICON_H
#define ICON_H

#include "../core/ui_types.h"

IconElement *ui_create_icon(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const uint8_t *iconData);

#endif // ICON_H