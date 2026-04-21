#include "ui_engine.h"
#include "display.h"
#include "font.h"
#include "esp_log.h"
#include <string.h>
#include "qrcode.h"
#include <stdint.h>

static const char *TAG = "DISPLAY";

void ui_draw_char(char c, int x, int y, Color color)
{
    if (c > 128)
        return;

    const uint8_t *char_bitmap = font[(int)c];

    for (int row = 0; row < 8; row++)
    {
        uint8_t line = char_bitmap[row];
        for (int col = 0; col < 8; col++)
        {
            if (line & (1 << col))
                display_set_pixel(x + col, y + row, color);
        }
    }
}

void ui_draw_string(const char *str, int x, int y, Color color)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        ui_draw_char(str[i], x + (i * 8), y, color);
    }
}

void ui_draw_shape(int x, int y, int w, int h, Color color)
{
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            display_set_pixel(x + i, y + j, color);
        }
    }
}

void ui_draw_border(int x, int y, int w, int h)
{
    for (int i = 0; i < w; i++)
    {
        display_set_pixel(x + i, y, true);
        display_set_pixel(x + i, y + h - 1, true);
    }
    for (int j = 0; j < h; j++)
    {
        display_set_pixel(x, y + j, true);
        display_set_pixel(x + w - 1, y + j, true);
    }
}

void draw_text_element(BaseElement *self)
{
    TextElement *el = (TextElement *)self;

    if (self->border)
    {
        ui_draw_border(self->x, self->y, self->w, self->h);
    }
    
    if (el->color == BLACK)
    {
        ui_draw_shape(self->x, self->y, self->w, self->h, true);
    }

    int x = self->x;
    int y = self->y;

    int text_height = 8;
    int text_width = strlen(el->text) * 8;

    if (self->xAnchor == CENTER)
    {
        x = self->x + (self->w - text_width) / 2;
    }
    else if (self->xAnchor == RIGHT)
    {
        x = self->x + self->w - text_width;
    }

    if (self->yAnchor == CENTER)
    {
        y = self->y + (self->h - text_height) / 2;
    }
    else if (self->yAnchor == BOTTOM)
    {
        y = self->y + self->h - text_height;
    }

    ui_draw_string(el->text, x, y, el->color);
}

void draw_qr_code_element(BaseElement *self)
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

                display_set_pixel(px, py, true);
                display_set_pixel(px + 1, py, true);
                display_set_pixel(px, py + 1, true);
                display_set_pixel(px + 1, py + 1, true);
            }
        }
    }
}

void ui_render_elements(BaseElement **elements, int count)
{
    int dirtyCount = 0;
    for (int i = 0; i < count; i++)
    {
        if (elements[i]->render && elements[i]->dirty)
        {
            ui_draw_shape(elements[i]->x, elements[i]->y, elements[i]->w, elements[i]->h, false);
            elements[i]->render(elements[i]);
            elements[i]->dirty = false;
            dirtyCount++;
        }
    }

    if (dirtyCount > 0)
    {
        display_update();
    }
}