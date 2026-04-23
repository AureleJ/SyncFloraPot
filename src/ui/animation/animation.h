#ifndef ANIMATION_H
#define ANIMATION_H

#include "easing.h"
#include "../core/ui_types.h"

typedef enum
{
    TRANSLATE_X,
    TRANSLATE_Y
} AnimationType;

typedef struct
{
    AnimationType type;
    int duration_ms;
    int delay_ms;
    int fromValue;
    int toValue;
    bool loop;
    BaseElement *target;
    EasingType easing;
    bool canStart;
    bool complete;
    uint32_t lastUpdate_ms;
} Animation;

Animation *ui_animate_element(BaseElement *element, AnimationType animation, int duration_ms, int delay_ms, int fromValue, int toValue, bool loop, EasingType easing);
void ui_start_animation(Animation *anim);
bool ui_is_animation_complete(Animation *anim);
void ui_update_animations(uint32_t current_time_ms);

#endif // ANIMATION_H