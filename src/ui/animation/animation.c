#include "animation.h"
#include <string.h>

#define MAX_ANIMATIONS 5
static Animation animation_pool[MAX_ANIMATIONS];

static int animation_pool_index = 0;

Animation *ui_animate_element(BaseElement *element, AnimationType animation, int duration_ms, int delay_ms, int fromValue, int toValue, bool loop, EasingType easing)
{
    if (animation_pool_index >= MAX_ANIMATIONS)
    {
        return NULL;
    }

    Animation *anim = &animation_pool[animation_pool_index++];
    anim->type = animation;
    anim->duration_ms = duration_ms;
    anim->delay_ms = delay_ms;
    anim->fromValue = fromValue;
    anim->toValue = toValue;
    anim->loop = loop;
    anim->target = element;
    anim->easing = easing;
    anim->canStart = false;
    anim->complete = false;
    anim->lastUpdate_ms = 0;

    return anim;
}

void ui_start_animation(Animation *anim)
{
    anim->canStart = true;
    anim->lastUpdate_ms = 0;
    anim->complete = false;
}

bool ui_is_animation_complete(Animation *anim)
{
    return anim->complete;
}

void ui_update_animations(uint32_t current_time_ms)
{
    for (int i = 0; i < animation_pool_index; i++)
    {
        Animation *anim = &animation_pool[i];
        if (!anim->canStart || anim->complete)
        {
            if (anim->loop && anim->complete)
            {
                ui_start_animation(anim);
            }

            continue;
        }

        if (anim->lastUpdate_ms == 0)
        {
            anim->lastUpdate_ms = current_time_ms;
        }
        else
        {
            int elapsed = current_time_ms - anim->lastUpdate_ms;
            if (elapsed >= anim->delay_ms)
            {
                float progress = (float)(elapsed - anim->delay_ms) / anim->duration_ms;

                progress = apply_easing(progress, anim->easing);

                if (progress >= 1.0f)
                {
                    progress = 1.0f;
                    anim->complete = true;
                }

                int newValue = anim->fromValue + (int)((anim->toValue - anim->fromValue) * progress);

                if (anim->type == TRANSLATE_X)
                {
                    anim->target->x = newValue;
                }
                else if (anim->type == TRANSLATE_Y)
                {
                    anim->target->y = newValue;
                }

                anim->target->dirty = true;
            }
        }
    }
}