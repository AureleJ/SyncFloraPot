#ifndef DIANUI_TYPES_H
#define DIANUI_TYPES_H

#include <stdbool.h>
#include <stdint.h>

/* Horizontal/vertical alignment of an element within its bounds */
typedef enum DianUI_Anchor
{
    DIANUI_TOP,
    DIANUI_LEFT,
    DIANUI_RIGHT,
    DIANUI_BOTTOM,
    DIANUI_CENTER
} DianUI_Anchor;

/* Pixel color - on monochrome displays, WHITE = on, BLACK = off */
typedef enum DianUI_Color
{
    DIANUI_BLACK,
    DIANUI_WHITE
} DianUI_Color;

/* Base element */
typedef struct DianUI_BaseElement
{
    int x, y;                                        /* Position in pixels */
    int w, h;                                        /* Size in pixels */
    DianUI_Anchor xAnchor;                           /* Horizontal alignment within bounds */
    DianUI_Anchor yAnchor;                           /* Vertical alignment within bounds */
    bool border;                                     /* Draw a border around the element */
    bool dirty;                                      /* Needs redraw on next dianui_render() */
    bool visible;                                    /* Hidden elements are skipped during render */
    void (*render)(struct DianUI_BaseElement *self); /* Render callback */
} DianUI_BaseElement;

/* 1-bit bitmap icon element */
typedef struct DianUI_IconElement
{
    DianUI_BaseElement base;
    const uint8_t *iconData; /* Pointer to bitmap data */
    int iconSize;            /* Icon width/height in pixels */
} DianUI_IconElement;

/* Text element */
typedef struct DianUI_TextElement
{
    DianUI_BaseElement base;
    const char *text;        /* Pointer to string */
    DianUI_Color color;      /* Text color */
    const uint8_t *iconData; /* Optional icon displayed before text */
    int iconSize;            /* Icon width/height in pixels */
    int iconGap;             /* Gap in pixels between icon and text */
} DianUI_TextElement;

/* Filled rectangle element */
typedef struct DianUI_ShapeElement
{
    DianUI_BaseElement base;
    DianUI_Color color; /* Shape color */
} DianUI_ShapeElement;

/* QR code element - data is encoded and rendered at draw time */
typedef struct DianUI_QRCodeElement
{
    DianUI_BaseElement base;
    const char *data; /* String to encode */
} DianUI_QRCodeElement;

#endif // DIANUI_TYPES_H