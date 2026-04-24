#ifndef DIANUI_ANIMATION_H
#define DIANUI_ANIMATION_H

#include "dianui_easing.h"
#include "../core/dianui_types.h"

typedef enum DianUI_AnimationType
{
    DIANUI_TRANSLATE_X,
    DIANUI_TRANSLATE_Y
} DianUI_AnimationType;

typedef struct DianUI_Animation
{
    DianUI_AnimationType type;
    int duration_ms;
    int delay_ms;
    int fromValue;
    int toValue;
    int currentValue;
    bool loop;
    DianUI_BaseElement *target;
    DianUI_EasingType easing;
    bool canStart;
    bool complete;
    uint32_t lastUpdate_ms;
} DianUI_Animation;

/**
 * @brief Create a new animation and attach it to an element.
 *
 * The returned animation is created in the internal animation pool and is
 * inactive until @ref dianui_start_animation is called.
 *
 * @param element Element to animate.
 * @param type Axis to animate (X or Y translation).
 * @param duration_ms Animation duration in milliseconds.
 * @param delay_ms Delay before animation starts, in milliseconds.
 * @param fromValue Start value applied to the target axis.
 * @param toValue End value applied to the target axis.
 * @param loop If true, the animation restarts automatically when complete.
 * @param easing Easing function used to interpolate progress.
 *
 * @return Pointer to the created animation, or NULL if the animation pool is full.
 */
DianUI_Animation *dianui_animate_element(DianUI_BaseElement *element, DianUI_AnimationType type, int duration_ms, int delay_ms, int fromValue, int toValue, bool loop, DianUI_EasingType easing);

/**
 * @brief Start an animation previously created with @ref dianui_animate_element.
 *
 * Resets internal runtime state so the animation can run from its initial value.
 *
 * @param anim Animation instance to start.
 */
void dianui_start_animation(DianUI_Animation *anim);

/**
 * @brief Check whether an animation has finished.
 *
 * @param anim Animation instance to query.
 *
 * @return true if the animation reached its end value, false otherwise.
 */
bool dianui_is_animation_complete(DianUI_Animation *anim);

/**
 * @brief Update all registered animations.
 *
 * Call this once per frame before rendering to apply the latest animation state
 * to target elements.
 *
 * @param current_time_ms Current time in milliseconds.
 */
void dianui_update_animations(uint32_t current_time_ms);

#endif // DIANUI_ANIMATION_H