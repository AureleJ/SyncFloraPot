#ifndef UI_ENGINE_H
#define UI_ENGINE_H

#include <stdbool.h>

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

void ui_render_elements(BaseElement **elements, int count);

void draw_text_element(BaseElement* self);

void draw_qr_code_element(BaseElement *self);

#endif // UI_ENGINE_H