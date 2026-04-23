#include "easing.h"

static float ease_linear(float t) {
    return t;
}

static float ease_in(float t) {
    return t * t * t * t; 
}

static float ease_out(float t) {
    float inv = 1.0f - t;
    return 1.0f - inv * inv * inv * inv;
}

static float ease_in_out(float t) {
    if (t < 0.5f) {
        return 8.0f * t * t * t * t;
    } else {
        float inv = 1.0f - t;
        return 1.0f - 8.0f * inv * inv * inv * inv;
    }
}

float apply_easing(float t, EasingType easing) {
    switch (easing) {
        case LINEAR:      return ease_linear(t);
        case EASE_IN:     return ease_in(t);
        case EASE_OUT:    return ease_out(t);
        case EASE_IN_OUT: return ease_in_out(t);
        default:          return t;
    }
}