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
    void (*set_pixel)(int x, int y, DianUI_Color color);          /* Set a single pixel */
    void (*set_hline)(int x1, int x2, int y, DianUI_Color color); /* Set a horizontal line */
    void (*clear)(void);                                          /* Clear the display buffer */
    void (*update)(void);                                         /* Flush buffer to display */
    void (*set_contrast)(uint8_t contrast);                       /* Set display contrast */
    void (*log)(const char *level, const char *message);          /* Log a message (for debugging) */
    int width;                                                    /* Display width in pixels */
    int height;                                                   /* Display height in pixels */
} DianUI_HAL;

#endif // DIANUI_HAL_H