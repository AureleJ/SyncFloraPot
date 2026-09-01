#ifndef DIANUI_EASING_H
#define DIANUI_EASING_H

typedef enum DianUI_EasingType
{
    // Linear
    DIANUI_LINEAR,

    // Ease in
    DIANUI_EASE_IN,

    // Ease out
    DIANUI_EASE_OUT_QUAD,
    DIANUI_EASE_OUT_CUBIC,
    DIANUI_EASE_OUT_BACK,

    // Ease in-out
    DIANUI_EASE_IN_OUT,
} DianUI_EasingType;

/**
 * @brief Apply an easing function to a normalized progress value.
 *
 * @param t Normalized progress (0.0 to 1.0).
 * @param easing Easing function to apply.
 * @return Adjusted progress value after applying easing.
 */
static inline float dianui_apply_easing(float t, DianUI_EasingType easing)
{
    if (t <= 0.0f)
        return 0.0f;
    if (t >= 1.0f)
        return 1.0f;

    switch (easing)
    {
    case DIANUI_EASE_IN:
        return t * t * t * t;

    case DIANUI_EASE_OUT_QUAD:
    {
        float inv = 1.0f - t;
        return 1.0f - inv * inv;
    }

    case DIANUI_EASE_OUT_CUBIC:
    {
        float inv = 1.0f - t;
        return 1.0f - inv * inv * inv;
    }

    case DIANUI_EASE_OUT_BACK:
    {
        float invT = 1.0f - t;
        return 1.0f - 1.3f * invT * invT * invT + 0.3f * invT;
    }

    case DIANUI_EASE_IN_OUT:
    {
        if (t < 0.5f)
        {
            return 8.0f * t * t * t * t;
        }
        else
        {
            float inv = 1.0f - t;
            return 1.0f - 8.0f * inv * inv * inv * inv;
        }
    }

    case DIANUI_LINEAR:
    default:
        return t;
    }
}

#endif // DIANUI_EASING_H