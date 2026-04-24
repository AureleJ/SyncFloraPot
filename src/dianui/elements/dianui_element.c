#include "dianui_element.h"

void dianui_set_visible(DianUI_BaseElement *self, bool visible)
{
    self->visible = visible;
}

void dianui_mark_dirty(DianUI_BaseElement *self)
{
    self->dirty = true;
}