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
    void (*set_pixel)(int16_t x, int16_t y, DianUI_Color color);          /* Set a single pixel */
    void (*set_hline)(int16_t x1, int16_t x2, int16_t y, DianUI_Color color); /* Set a horizontal line */
    void (*set_vline)(int16_t x, int16_t y1, int16_t y2, DianUI_Color color); /* Set a vertical line */
    void (*clear)(void);                                          /* Clear the display buffer */
    void (*update)(void);                                         /* Flush buffer to display */
    void (*set_contrast)(uint8_t contrast);                       /* Set display contrast */
    void (*log)(const char *level, const char *message);          /* Log a message (for debugging) */
    uint16_t width;                                               /* Display width in pixels */
    uint16_t height;                                              /* Display height in pixels */
} DianUI_HAL;

#endif // DIANUI_HAL_H