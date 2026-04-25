#include "dianui_qrcode.h"
#include "../core/dianui_engine_private.h"
#include "../core/dianui_renderer.h"
#include "../core/dianui_log.h"
#include "../core/dianui_config.h"
#include <string.h>
#include "qrcode_generator.h"

static DianUI_QRCodeElement qr_pool[DIANUI_MAX_QR];
static int qr_pool_index = 0;

static void draw_qr_code_element(DianUI_BaseElement *self);

DianUI_QRCodeElement *dianui_create_qr_code(int x, int y, int w, int h, DianUI_Anchor xAnchor, DianUI_Anchor yAnchor, const char *data)
{
    if (qr_pool_index >= DIANUI_MAX_QR)
    {
        return NULL;
    }

    DianUI_QRCodeElement *el = &qr_pool[qr_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_qr_code_element;
    el->base.visible = true;
    el->data = data;

    dianui_engine_register((DianUI_BaseElement *)el);

    return el;
}

static void draw_qr_code_element(DianUI_BaseElement *self)
{
    DianUI_QRCodeElement *el = (DianUI_QRCodeElement *)self;

    QRCode qrcode;
    qrcode_init(&qrcode, el->data);

    int oldX = self->x;
    int oldY = self->y;

    int text_height = qrcode.size * 2;
    int text_width = qrcode.size * 2;

    if (self->xAnchor == DIANUI_CENTER)
    {
        oldX = self->x + (self->w - text_width) / 2;
    }
    else if (self->xAnchor == DIANUI_RIGHT)
    {
        oldX = self->x + self->w - text_width;
    }

    if (self->yAnchor == DIANUI_CENTER)
    {
        oldY = self->y + (self->h - text_height) / 2;
    }
    else if (self->yAnchor == DIANUI_BOTTOM)
    {
        oldY = self->y + self->h - text_height;
    }

    for (int y = 0; y < qrcode.size; y++)
    {
        for (int x = 0; x < qrcode.size; x++)
        {
            if (qrcode_getModule(&qrcode, x, y))
            {
                int px = oldX + x * 2;
                int py = oldY + y * 2;

                dianui_draw_pixel(px, py, DIANUI_WHITE);
                dianui_draw_pixel(px + 1, py, DIANUI_WHITE);
                dianui_draw_pixel(px, py + 1, DIANUI_WHITE);
                dianui_draw_pixel(px + 1, py + 1, DIANUI_WHITE);
            }
        }
    }
}

void dianui_qrcode_reset()
{
    qr_pool_index = 0;
}