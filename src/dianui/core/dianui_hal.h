#ifndef DIANUI_HAL_H
#define DIANUI_HAL_H

#include "dianui_types.h"

/**
 * Hardware Abstraction Layer (HAL) interface.
 *
 * The engine relies on the HAL to interact with the display hardware.
 */
typedef struct DianUI_HAL
{
    void (*set_pixel)(int x, int y, DianUI_Color color); /* Set a single pixel */
    void (*clear)(void);                                 /* Clear the display buffer */
    void (*update)(void);                                /* Flush buffer to display */
    void (*log)(const char *level, const char *message); /* Log a message (for debugging) */
    int width;                                           /* Display width in pixels */
    int height;                                          /* Display height in pixels */
} DianUI_HAL;

#endif // DIANUI_HAL_H