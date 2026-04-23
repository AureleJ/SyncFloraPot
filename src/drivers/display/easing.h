#ifndef EASING_H
#define EASING_H

typedef enum EasingType{
    LINEAR,
    EASE_IN,
    EASE_OUT,
    EASE_IN_OUT
} EasingType;

float apply_easing(float t, EasingType easing);

#endif // EASING_H