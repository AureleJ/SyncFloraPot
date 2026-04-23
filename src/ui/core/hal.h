#ifndef UI_HAL_H
#define UI_HAL_H

#include "ui_types.h"
#include <stdbool.h>

typedef struct UI_HAL
{
    void (*set_pixel)(int x, int y, Color color);
    void (*clear)(void);
    void (*update)(void);
    int width;
    int height;
} UI_HAL;

#endif // UI_HAL_H