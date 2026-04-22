#ifndef UI_ENGINE_H
#define UI_ENGINE_H

#include <stdbool.h>
#include <stdint.h>

typedef enum Anchor
{
    TOP,
    LEFT,
    RIGHT,
    BOTTOM,
    CENTER
} Anchor;

typedef enum Color
{
    BLACK,
    WHITE
} Color;

typedef struct BaseElement
{
    int x, y;
    int w, h;
    Anchor xAnchor;
    Anchor yAnchor;
    bool border;
    bool dirty : 1;
    void (*render)(struct BaseElement *self);
} BaseElement;

typedef struct TextElement
{
    BaseElement base;
    const char *text;
    Color color;
} TextElement;

typedef struct ShapeElement
{
    BaseElement base;
    Color color;
} ShapeElement;

typedef struct QRCodeElement
{
    BaseElement base;
    const char *data;
} QRCodeElement;

typedef struct IconElement
{
    BaseElement base;
    const uint8_t *iconData;
} IconElement;

void ui_render();

void ui_clear();

TextElement *ui_create_text(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *text, Color color);

ShapeElement *ui_create_shape(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, Color color);

QRCodeElement *ui_create_qr_code(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *data);

IconElement *ui_create_icon(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const uint8_t *iconData);

#endif // UI_ENGINE_H