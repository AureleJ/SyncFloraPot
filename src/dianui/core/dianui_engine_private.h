#ifndef DIANUI_ENGINE_PRIVATE_H
#define DIANUI_ENGINE_PRIVATE_H

#include "dianui_types.h"
#include "dianui_hal.h"

/**
 * @brief Register a new element.
 *
 * @param el Pointer to the element to register.
 */
void dianui_engine_register(DianUI_BaseElement *el);

/**
 * @brief Get the width of the screen.
 *
 * @return The width of the screen.
 */
int dianui_get_screen_width();

/**
 * @brief Get the height of the screen.
 *
 * @return The height of the screen.
 */
int dianui_get_screen_height();

/**
 * @brief Get the HAL instance.
 *
 * @return Pointer to the HAL instance.
 */
DianUI_HAL *dianui_engine_get_hal();

#endif // DIANUI_ENGINE_PRIVATE_H