/**
 * @file dianui.h
 * @brief Main public header for the DianUI engine.
 *
 * DianUI (點UI) is a lightweight UI engine for embedded systems with monochrome displays.
 */

#ifndef DIANUI_H
#define DIANUI_H

/* Core */
#include "core/dianui_types.h"
#include "core/dianui_hal.h"
#include "core/dianui_engine.h"
#include "core/dianui_time.h"

/* Elements */
#include "elements/dianui_element.h"

/* Widgets */
#include "widget/dianui_widget.h"

/* Animations */
#include "animation/dianui_animation.h"

/* Router and Pages */
#include "pages/dianui_pages.h"

#endif // DIANUI_H