#ifndef DIANUI_POLYGON_H
#define DIANUI_POLYGON_H

#include "../core/dianui_types.h"

DianUI_PolygonElement *dianui_create_polygon(DianUI_Point pos, const DianUI_Corner corners[4], DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, DianUI_Color color);

/**
 * @brief Reset the polygons element pool.
 */
void dianui_polygons_reset();

#endif // DIANUI_POLYGON_H