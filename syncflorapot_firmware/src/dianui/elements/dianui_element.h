#ifndef DIANUI_ELEMENT_H
#define DIANUI_ELEMENT_H

#include "../core/dianui_types.h"
#include "dianui_icon.h"
#include "dianui_text.h"
#include "dianui_shape.h"
#include "dianui_qrcode.h"

/**
 * @brief Set the visibility of an element.
 *
 * @param self Pointer to the element.
 * @param visible Boolean indicating whether the element should be visible.
 */
void dianui_set_visible(DianUI_BaseElement *self, bool visible);

/**
 * @brief Mark an element as dirty, forcing a redraw on the next call.
 *
 * @param self Pointer to the element.
 */
void dianui_mark_dirty(DianUI_BaseElement *self);

/**
 * @brief Reset all element pools.
 */
void dianui_elements_reset();

#endif // DIANUI_ELEMENT_H