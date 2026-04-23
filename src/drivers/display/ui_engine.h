#ifndef UI_ENGINE_H
#define UI_ENGINE_H

#include <stdbool.h>
#include <stdint.h>
#include "easing.h"

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

typedef enum AnimationType
{
    TRANSLATE_X,
    TRANSLATE_Y
} AnimationType;

typedef struct BaseElement
{
    int x, y;
    int w, h;
    Anchor xAnchor;
    Anchor yAnchor;
    bool border;
    bool dirty : 1;
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

typedef struct Animation
{
    BaseElement *target;
    AnimationType type;
    bool canStart;
    bool complete;
    int duration_ms;
    int lastUpdate_ms;
    int delay_ms;
    int fromValue;
    int toValue;
    bool loop;
    EasingType easing;
} Animation;

void ui_render();

void ui_clear();

TextElement *ui_create_text(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *text, Color color);

ShapeElement *ui_create_shape(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, Color color);

QRCodeElement *ui_create_qr_code(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const char *data);

IconElement *ui_create_icon(int x, int y, int w, int h, Anchor xAnchor, Anchor yAnchor, const uint8_t *iconData);

Animation *ui_animate_element(BaseElement *element, AnimationType animation, int duration_ms, int delay_ms, int fromValue, int toValue, bool loop, EasingType easing);

void ui_start_animation(Animation *anim);

void ui_update_animations(uint32_t current_time_ms);

bool ui_is_animation_complete(Animation *anim);

void ui_set_element_invisible(BaseElement *self);

void ui_add_icon_to_text(TextElement *textEl, const uint8_t *iconData, int gap);

#endif // UI_ENGINE_H