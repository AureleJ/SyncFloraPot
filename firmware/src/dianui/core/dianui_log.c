#include "dianui_log.h"
#include "dianui_engine_private.h" 

void dianui_log_write(const char *level, const char *msg) {
    DianUI_HAL *hal = dianui_engine_get_hal();
    if (hal && hal->log)
        hal->log(level, msg);
}