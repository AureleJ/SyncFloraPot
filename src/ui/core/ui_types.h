#ifndef UI_TYPES_H
#define UI_TYPES_H

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
    bool dirty;
    bool visible;
    void (*render)(struct BaseElement *self);
} BaseElement;

typedef struct IconElement
{
    BaseElement base;
    const uint8_t *iconData;
} IconElement;

typedef struct TextElement
{
    BaseElement base;
    const char *text;
    Color color;
    const uint8_t *iconData; // Optional icon data for text elements
    int iconGap;
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

#endif // UI_TYPES_H