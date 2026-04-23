#include "element.h"
#include "qrcode.h"
#include "../core/engine_private.h"
#include "../core/renderer.h"
#include <string.h>
#include "qrcode_generator.h"

#define MAX_QR_ELEMENTS 5

static QRCodeElement qr_pool[MAX_QR_ELEMENTS];
static int qr_pool_index = 0;

static void draw_qr_code_element(BaseElement *self);

QRCodeElement *ui_create_qr_code(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *data)
{
    if (qr_pool_index >= MAX_QR_ELEMENTS)
    {
        return NULL;
    }

    QRCodeElement *el = &qr_pool[qr_pool_index++];
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

    engine_register_element((BaseElement *)el);

    return el;
}

static void draw_qr_code_element(BaseElement *self)
{
    QRCodeElement *el = (QRCodeElement *)self;

    QRCode qrcode;
    qrcode_init(&qrcode, el->data);

    int oldX = self->x;
    int oldY = self->y;

    int text_height = qrcode.size * 2;
    int text_width = qrcode.size * 2;

    if (self->xAnchor == CENTER)
    {
        oldX = self->x + (self->w - text_width) / 2;
    }
    else if (self->xAnchor == RIGHT)
    {
        oldX = self->x + self->w - text_width;
    }

    if (self->yAnchor == CENTER)
    {
        oldY = self->y + (self->h - text_height) / 2;
    }
    else if (self->yAnchor == BOTTOM)
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

                ui_draw_pixel(px, py, WHITE);
                ui_draw_pixel(px + 1, py, WHITE);
                ui_draw_pixel(px, py + 1, WHITE);
                ui_draw_pixel(px + 1, py + 1, WHITE);
            }
        }
    }
}
