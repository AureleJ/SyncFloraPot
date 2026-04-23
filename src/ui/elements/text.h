#ifndef TEXT_H
#define TEXT_H

#include "../core/ui_types.h"

TextElement *ui_create_text(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *text, Color color);
void ui_add_icon_to_text(TextElement *textEl, const uint8_t *iconData, int gap);

#endif // TEXT_H