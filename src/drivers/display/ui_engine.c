#include "ui_engine.h"
#include "display.h"
#include "font.h"
#include "esp_log.h"
#include "qrcode.h"
#include <string.h>
#include <stdint.h>

static const char *TAG = "DISPLAY";

// Can be increased if needed
#define MAX_UI_ELEMENTS 10 
#define MAX_TEXT_ELEMENTS 10
#define MAX_SHAPE_ELEMENTS 10
#define MAX_QR_ELEMENTS 5
#define MAX_ICON_ELEMENTS 5

static BaseElement *display_registry[MAX_UI_ELEMENTS];
static int element_count = 0;

static TextElement text_pool[MAX_TEXT_ELEMENTS];
static int text_pool_index = 0;

static ShapeElement shape_pool[MAX_SHAPE_ELEMENTS];
static int shape_pool_index = 0;

static QRCodeElement qr_pool[MAX_QR_ELEMENTS];
static int qr_pool_index = 0;

static IconElement icon_pool[MAX_ICON_ELEMENTS];
static int icon_pool_index = 0;

static void draw_text_element(BaseElement *self);
static void draw_shape_element(BaseElement *self);
static void draw_qr_code_element(BaseElement *self);
static void draw_icon_element(BaseElement *self);

static void ui_draw_string(const char *str, int x, int y, Color color);
static void ui_draw_shape(int x, int y, int w, int h, Color color);
static void ui_draw_border(int x, int y, int w, int h);
static void ui_draw_char(char c, int x, int y, Color color);

/* ELEMENT CREATION */

TextElement *ui_create_text(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *text, Color color)
{
    if (element_count >= MAX_UI_ELEMENTS || text_pool_index >= MAX_TEXT_ELEMENTS)
    {
        ESP_LOGE(TAG, "Max UI elements reached, cannot create more");
        return NULL;
    }

    TextElement *el = &text_pool[text_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_text_element;
    el->text = text;
    el->color = color;

    display_registry[element_count++] = (BaseElement *)el;

    return el;
}

ShapeElement *ui_create_shape(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, Color color)
{
    if (element_count >= MAX_UI_ELEMENTS || shape_pool_index >= MAX_SHAPE_ELEMENTS)
    {
        ESP_LOGE(TAG, "Max UI elements reached, cannot create more");
        return NULL;
    }

    ShapeElement *el = &shape_pool[shape_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_shape_element;
    el->color = color;

    display_registry[element_count++] = (BaseElement *)el;

    return el;
}

QRCodeElement *ui_create_qr_code(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *data)
{
    if (element_count >= MAX_UI_ELEMENTS || qr_pool_index >= MAX_QR_ELEMENTS)
    {
        ESP_LOGE(TAG, "Max UI elements reached, cannot create more");
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
    el->data = data;

    display_registry[element_count++] = (BaseElement *)el;

    return el;
}

IconElement *ui_create_icon(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const uint8_t *iconData)
{
    if (element_count >= MAX_UI_ELEMENTS || icon_pool_index >= MAX_ICON_ELEMENTS)
    {
        ESP_LOGE(TAG, "Max UI elements reached, cannot create more");
        return NULL;
    }

    IconElement *el = &icon_pool[icon_pool_index++];
    el->base.x = x;
    el->base.y = y;
    el->base.w = w;
    el->base.h = h;
    el->base.xAnchor = xAnchor;
    el->base.yAnchor = yAnchor;
    el->base.border = false;
    el->base.dirty = true;
    el->base.render = draw_icon_element;
    el->iconData = iconData;

    display_registry[element_count++] = (BaseElement *)el;

    return el;
}

/* ELEMENT RENDERERS */

static void draw_text_element(BaseElement *self)
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

                display_set_pixel(px, py, true);
                display_set_pixel(px + 1, py, true);
                display_set_pixel(px, py + 1, true);
                display_set_pixel(px + 1, py + 1, true);
            }
        }
    }
}

static void draw_icon_element(BaseElement *self)
{
    IconElement *el = (IconElement *)self;

    for (int y = 0; y < 8; y++)
    {
        uint8_t line = el->iconData[y];
        for (int x = 0; x < 8; x++)
        {
            if (line & (1 << x))
            {
                display_set_pixel(self->x + x, self->y + y, true);
            }
        }
    }
}

static void draw_shape_element(BaseElement *self)
{
    ShapeElement *el = (ShapeElement *)self;

    ui_draw_shape(self->x, self->y, self->w, self->h, el->color);

    if (self->border)
    {
        ui_draw_border(self->x, self->y, self->w, self->h);
    }
}

/* RENDERING */

void ui_render()
{
    int dirtyCount = 0;
    for (int i = 0; i < element_count; i++)
    {
        if (display_registry[i]->render && display_registry[i]->dirty)
        {
            ui_draw_shape(display_registry[i]->x, display_registry[i]->y, display_registry[i]->w, display_registry[i]->h, false);
            display_registry[i]->render(display_registry[i]);
            display_registry[i]->dirty = false;
            dirtyCount++;
        }
    }

    if (dirtyCount > 0)
    {
        display_update();
    }
}

void ui_clear()
{
    display_clear();
    display_update();
}

/* LOW-LEVEL DRAWING */

static void ui_draw_char(char c, int x, int y, Color color)
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

static void ui_draw_string(const char *str, int x, int y, Color color)
{
    for (int i = 0; str[i] != '\0'; i++)
    {
        ui_draw_char(str[i], x + (i * 8), y, color);
    }
}

static void ui_draw_shape(int x, int y, int w, int h, Color color)
{
    for (int j = 0; j < h; j++)
    {
        for (int i = 0; i < w; i++)
        {
            display_set_pixel(x + i, y + j, color);
        }
    }
}

static void ui_draw_border(int x, int y, int w, int h)
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