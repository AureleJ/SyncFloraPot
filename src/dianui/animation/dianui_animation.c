#include "dianui_animation.h"
#include "../core/dianui_config.h"
#include <string.h>

static DianUI_Animation animation_pool[DIANUI_MAX_ANIMATIONS];
static int animation_pool_index = 0;

DianUI_Animation *dianui_animate_element(DianUI_BaseElement *element, DianUI_AnimationType type, int duration_ms, int delay_ms, int fromValue, int toValue, bool loop, DianUI_EasingType easing)
{
    if (animation_pool_index >= DIANUI_MAX_ANIMATIONS)
        return NULL;

    DianUI_Animation *anim = &animation_pool[animation_pool_index++];
    anim->type = type;
    anim->duration_ms = duration_ms;
    anim->delay_ms = delay_ms;
    anim->fromValue = fromValue;
    anim->toValue = toValue;
    anim->currentValue = fromValue;
    anim->loop = loop;
    anim->target = element;
    anim->easing = easing;
    anim->canStart = false;
    anim->complete = false;
    anim->lastUpdate_ms = 0;

    return anim;
}

void dianui_start_animation(DianUI_Animation *anim)
{
    anim->canStart = true;
    anim->lastUpdate_ms = 0;
    anim->complete = false;
}

bool dianui_is_animation_complete(DianUI_Animation *anim)
{
    return anim->complete;
}

void dianui_update_animations(uint32_t current_time_ms)
{
    for (int i = 0; i < animation_pool_index; i++)
    {
        DianUI_Animation *anim = &animation_pool[i];

        if (!anim->canStart || anim->complete)
        {
            if (anim->loop && anim->complete)
                dianui_start_animation(anim);
            continue;
        }

        if (anim->lastUpdate_ms == 0)
        {
            anim->lastUpdate_ms = current_time_ms;
            continue;
        }

        int elapsed = current_time_ms - anim->lastUpdate_ms;
        if (elapsed < anim->delay_ms)
            continue;

        float progress = (float)(elapsed - anim->delay_ms) / anim->duration_ms;
        progress = dianui_apply_easing(progress, anim->easing);

        if (progress >= 1.0f)
        {
            progress = 1.0f;
            anim->complete = true;
        }

        int newValue = anim->fromValue + (int)((anim->toValue - anim->fromValue) * progress);
        anim->currentValue = newValue;

        if (anim->type == DIANUI_TRANSLATE_X)
            anim->target->x = newValue;
        else if (anim->type == DIANUI_TRANSLATE_Y)
            anim->target->y = newValue;

        anim->target->dirty = true;
    }
}