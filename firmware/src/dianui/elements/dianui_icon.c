#include "dianui_icon.h"
#include "../core/dianui_engine_private.h"
#include "../core/dianui_renderer.h"
#include "../core/dianui_config.h"
#include "../core/dianui_log.h"
#include <string.h>

static DianUI_IconElement icon_pool[DIANUI_MAX_ICONS];
static uint8_t icon_pool_index = 0;

static void draw_icon_element(DianUI_BaseElement *self);

DianUI_IconElement *dianui_create_icon(int16_t x, int16_t y, int16_t w, int16_t h, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, const uint8_t *iconData, uint8_t iconSize)
{
    if (icon_pool_index >= DIANUI_MAX_ICONS || !iconData || iconSize == 0)
    {
        DIANUI_LOGE("Icon pool exhausted or invalid icon data");
        return NULL;
    }

    DianUI_IconElement *el = &icon_pool[icon_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_icon_element;
    el->base.visible = true;
    el->iconData = iconData;
    el->iconSize = iconSize;

    dianui_engine_register((DianUI_BaseElement *)el);

    return el;
}

DianUI_IconElement *dianui_update_icon(DianUI_IconElement *icon, const uint8_t *iconData, uint8_t iconSize)
{
    if (!icon || !iconData || iconSize == 0)
    {
        DIANUI_LOGE("Invalid icon or icon data");
        return NULL;
    }

    icon->iconData = iconData;
    icon->iconSize = iconSize;
    icon->base.dirty = true;

    return icon;
}


static void draw_icon_element(DianUI_BaseElement *self)
{
    if (!self)
    {
        DIANUI_LOGE("DianUI element is NULL.");
        return;
    }
    DianUI_IconElement *el = (DianUI_IconElement *)self;

    dianui_draw_icon(el->iconData, el->base.x, el->base.y, el->iconSize, DIANUI_WHITE);
}

void dianui_icon_reset()
{
    icon_pool_index = 0;
}