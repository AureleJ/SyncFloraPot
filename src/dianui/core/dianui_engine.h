#ifndef DIANUI_ENGINE_H
#define DIANUI_ENGINE_H

#include "dianui_hal.h"
#include "dianui_types.h"

/**
 * @brief Initialize the engine with a HAL implementation.
 *
 * @param hal_ptr Pointer to the HAL implementation.
 */
void dianui_init(DianUI_HAL *hal_ptr);

/**
 * @brief Redraw all dirty elements and flush to display.
 */
void dianui_render(void);

/**
 * @brief Clear the display immediately.
 */
void dianui_clear(void);

#endif // DIANUI_ENGINE_H