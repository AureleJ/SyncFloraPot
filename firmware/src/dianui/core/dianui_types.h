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

typedef enum DianUI_ShapeType
{
    DIANUI_SHAPE_RECTANGLE,
    DIANUI_SHAPE_ROUNDED_RECTANGLE,
    DIANUI_SHAPE_CIRCLE,
    DIANUI_SHAPE_LINE
} DianUI_ShapeType;

/* Point */
typedef struct DianUI_Point
{
    int x, y;
} DianUI_Point;

/* Size */
typedef struct DianUI_Size
{
    int w, h;
} DianUI_Size;

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
typedef struct DianUI_RectangleElement
{
    DianUI_BaseElement base;
    DianUI_Color color; /* Fill color */
    uint8_t radius;     /* Corner radius for rounded rectangles (0 = sharp) */
} DianUI_RectangleElement;

/* Circle element */
typedef struct DianUI_CircleElement
{
    DianUI_BaseElement base;
    DianUI_Color color; /* Fill color */
    uint8_t radius;     /* Radius of the circle */
} DianUI_CircleElement;

/* Line element */
typedef struct DianUI_LineElement
{
    DianUI_BaseElement base;
    DianUI_Color color; /* Line color */
    uint8_t thickness;  /* Line thickness in pixels */
    uint16_t x2, y2;    /* End point coordinates */
} DianUI_LineElement;

/* Triangle element */
typedef struct DianUI_TriangleElement
{
    DianUI_BaseElement base;
    DianUI_Color color; /* Fill color */
} DianUI_TriangleElement;

/* Polygon 4 points element */
typedef struct
{
    DianUI_Point pos;
    int roundness;
} DianUI_Corner;

typedef struct
{
    DianUI_BaseElement base;
    DianUI_Color color;
    DianUI_Point pos;
    DianUI_Corner corners[4];
} DianUI_PolygonElement;

/* QR code element - data is encoded and rendered at draw time */
typedef struct DianUI_QRCodeElement
{
    DianUI_BaseElement base;
    const char *data; /* String to encode */
} DianUI_QRCodeElement;

/* Page element */
typedef enum DianUI_NavEvent
{
    DIANUI_NAV_NONE = 0,
    DIANUI_NAV_PREV,
    DIANUI_NAV_NEXT,
    DIANUI_NAV_SELECT,
    DIANUI_NAV_BACK
} DianUI_NavEvent;

typedef struct DianUI_Page
{
    const char *name;
    void (*on_enter)(void);
    void (*on_update)(float dt);
    void (*on_input)(DianUI_NavEvent event);
    void (*on_exit)(void);
} DianUI_Page;

#endif // DIANUI_TYPES_H