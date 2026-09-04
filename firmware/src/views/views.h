#ifndef VIEWS_H
#define VIEWS_H

#include "dianui/core/dianui_types.h"
#include "dianui/dianui.h"
#include <stdio.h>

typedef enum {
    VIEW_ID_HOME = 0,
    VIEW_ID_SETTINGS,
    VIEW_ID_LOADING,
    VIEW_ID_ABOUT,
    VIEW_ID_PROVISIONNING,
    VIEW_ID_ERROR,
    VIEW_ID_CONNECTING,
    VIEW_ID_COUNT
} ViewId;

extern const DianUI_Page view_home;
extern const DianUI_Page view_settings;
extern const DianUI_Page view_loading;
extern const DianUI_Page view_about;
extern const DianUI_Page view_provisionning;
extern const DianUI_Page view_error;
extern const DianUI_Page view_connecting;

#endif // VIEWS_H