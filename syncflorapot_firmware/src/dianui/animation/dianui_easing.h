#ifndef DIANUI_EASING_H
#define DIANUI_EASING_H

typedef enum DianUI_EasingType
{
    DIANUI_LINEAR,
    DIANUI_EASE_IN,
    DIANUI_EASE_OUT,
    DIANUI_EASE_IN_OUT
} DianUI_EasingType;

/**
 * @brief Apply an easing function to a normalized progress value.
 *
 * @param t Normalized progress (0.0 to 1.0).
 * @param easing Easing function to apply.
 * @return Adjusted progress value after applying easing.
 */
float dianui_apply_easing(float t, DianUI_EasingType easing);

#endif // DIANUI_EASING_H