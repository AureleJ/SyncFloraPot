#ifndef UI_ENGINE_H
#define UI_ENGINE_H

#include "hal.h"
#include "ui_types.h"

void ui_init(UI_HAL *hal_ptr);

void ui_render();

void ui_clear();

#endif // UI_ENGINE_H