#ifndef DIANUI_LOG_H
#define DIANUI_LOG_H

#include "dianui_config.h"

/**
 * DianUI logging system.
 * Provides logging at different levels (ERROR, WARN, INFO).
 */
#if DIANUI_LOG_ENABLED
    #define DIANUI_LOGE(msg) dianui_log_write("ERROR", msg)
    #define DIANUI_LOGW(msg) dianui_log_write("WARN",  msg)
    #define DIANUI_LOGI(msg) dianui_log_write("INFO",  msg)
#else
    #define DIANUI_LOGE(msg)
    #define DIANUI_LOGW(msg)
    #define DIANUI_LOGI(msg)
#endif

void dianui_log_write(const char *level, const char *msg);

#endif // DIANUI_LOG_H