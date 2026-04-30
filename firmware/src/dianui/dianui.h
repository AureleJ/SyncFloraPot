/**
 * @file dianui.h
 * @brief Main public header for the DianUI engine.
 *
 * DianUI (DiǎnUI or 點UI) is a lightweight UI engine for embedded systems with monochrome displays.
 */

#ifndef UI_H
#define UI_H

// Core
#include "core/dianui_types.h"
#include "core/dianui_hal.h"
#include "core/dianui_engine.h"

// Elements
#include "elements/dianui_element.h"
#include "elements/dianui_icon.h"
#include "elements/dianui_text.h"
#include "elements/dianui_shape.h"
#include "elements/dianui_qrcode.h"

// Animations
#include "animation/dianui_animation.h"

// Pages
#include "pages/dianui_pages.h"

#endif // UI_H