#ifndef SHAPE_H
#define SHAPE_H

#include "../core/ui_types.h"

ShapeElement *ui_create_shape(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, Color color);

#endif // SHAPE_H