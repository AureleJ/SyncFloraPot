#include "dianui_element.h"
#include "../core/dianui_log.h"

void dianui_set_visible(DianUI_BaseElement *self, bool visible)
{
    if (!self)
    {
        DIANUI_LOGE("DianUI element is NULL.");
        return;
    }

    self->visible = visible;
}

void dianui_mark_dirty(DianUI_BaseElement *self)
{
    if (!self)
    {
        DIANUI_LOGE("DianUI element is NULL.");
        return;
    }
    
    self->dirty = true;
}

void dianui_elements_reset()
{
    dianui_icon_reset();
    dianui_text_reset();
    dianui_shape_reset();
    dianui_qrcode_reset();
}