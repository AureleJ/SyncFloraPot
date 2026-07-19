#ifndef DIANUI_FACE_H
#define DIANUI_FACE_H

#include "../core/dianui_types.h"

typedef enum
{
    IDLE,
    HAPPY,
    BLINK,
    SLEEPY
} Emotion;

void dianui_face_init();

void dianui_update_face(uint32_t now);

void dianui_set_emotion(Emotion emotion);

#endif // DIANUI_FACE_H