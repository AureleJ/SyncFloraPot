// Faire l'interpolation entre des emotions
// Faire le regard dans differents directions
// Faire le clignement des yeux

#include "dianui_face.h"
#include "dianui/core/dianui_types.h"
#include "dianui/dianui.h"
#include "math.h"

DianUI_PolygonElement *leftEye = 0;
DianUI_PolygonElement *rightEye = 0;

Emotion currentEmotion = IDLE;
Emotion targetEmotion = IDLE;

// Blink
DianUI_Corner blinkEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 4},
    {.pos = {.x = 40, .y = 0}, .roundness = 4},
    {.pos = {.x = 40, .y = 3}, .roundness = 4},
    {.pos = {.x = 0, .y = 3}, .roundness = 4},
};

// Idle
DianUI_Corner normalEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 30}, .roundness = 8},
    {.pos = {.x = 0, .y = 30}, .roundness = 8},
};

// Happy
DianUI_Corner happyEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 8}, .roundness = 0},
    {.pos = {.x = 0, .y = 8}, .roundness = 0}
};

// Sleepy
DianUI_Corner sleepyEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 3},
    {.pos = {.x = 35, .y = 0}, .roundness = 3},
    {.pos = {.x = 35, .y = 5}, .roundness = 3},
    {.pos = {.x = 0, .y = 5}, .roundness = 3}
};

/*
DianUI_Corner shockedEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 10},
    {.pos = {.x = 20, .y = 0}, .roundness = 10},
    {.pos = {.x = 20, .y = 30}, .roundness = 10},
    {.pos = {.x = 0, .y = 30}, .roundness = 10}};

DianUI_Corner sadEyeLeft[4] = {
    {.pos = {.x = 0, .y = 4}, .roundness = 6},
    {.pos = {.x = 20, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 12}, .roundness = 4},
    {.pos = {.x = 0, .y = 12}, .roundness = 4}};

DianUI_Corner sadEyeRight[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 4}, .roundness = 6},
    {.pos = {.x = 20, .y = 12}, .roundness = 4},
    {.pos = {.x = 0, .y = 12}, .roundness = 4}};

DianUI_Corner angryEyeLeft[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 0},
    {.pos = {.x = 20, .y = 6}, .roundness = 0},
    {.pos = {.x = 20, .y = 14}, .roundness = 4},
    {.pos = {.x = 0, .y = 14}, .roundness = 4}};

DianUI_Corner angryEyeRight[4] = {
    {.pos = {.x = 0, .y = 6}, .roundness = 0},
    {.pos = {.x = 20, .y = 0}, .roundness = 0},
    {.pos = {.x = 20, .y = 14}, .roundness = 4},
    {.pos = {.x = 0, .y = 14}, .roundness = 4}};

DianUI_Corner suspiciousEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 3},
    {.pos = {.x = 20, .y = 0}, .roundness = 3},
    {.pos = {.x = 20, .y = 4}, .roundness = 2},
    {.pos = {.x = 0, .y = 4}, .roundness = 2}};

DianUI_Corner winkEye[4] = {
    {.pos = {.x = 0, .y = 6}, .roundness = 2},
    {.pos = {.x = 10, .y = 0}, .roundness = 8},
    {.pos = {.x = 20, .y = 6}, .roundness = 2},
    {.pos = {.x = 10, .y = 4}, .roundness = 2}}; */

DianUI_TextElement *textEl = 0;

void dianui_face_init()
{
    leftEye = dianui_create_polygon(
        (DianUI_Point){.x = 50, .y = 40},
        normalEye,
        DIANUI_CENTER,
        DIANUI_CENTER,
        DIANUI_WHITE);

    rightEye = dianui_create_polygon(
        (DianUI_Point){.x = 90, .y = 40},
        normalEye,
        DIANUI_CENTER,
        DIANUI_CENTER,
        DIANUI_WHITE);

    textEl = dianui_create_text(10, 10, 128, 64, DIANUI_LEFT, DIANUI_TOP, "Zzz", DIANUI_WHITE);
    dianui_set_visible((DianUI_BaseElement *)textEl, false);
}

int targetEyePosX = 0;
int currentEyePosX = 0;
uint32_t nextLookTime = 0;

uint32_t nextBlinkTime = 0;
bool isBlinking = false;
uint32_t blinkStartTime = 0;
#define BLINK_DURATION 160

uint32_t nextEmotionChangeTime = 0;

void dianui_set_emotion(Emotion emotion)
{
    targetEmotion = emotion;
}

const DianUI_Corner *EMOTION_REGISTRY[] = {
    [IDLE] = normalEye,
    [HAPPY] = happyEye,
    [BLINK] = blinkEye,
    [SLEEPY] = sleepyEye};

// Implémentation brute a revoir
void dianui_interpolate_emotion(DianUI_PolygonElement *polygon, Emotion from, Emotion to, float t)
{
    const DianUI_Corner *fromCorners = EMOTION_REGISTRY[from];
    const DianUI_Corner *toCorners = EMOTION_REGISTRY[to];

    DianUI_Corner *currentCorners = polygon->corners;

    for (int i = 0; i < 4; i++)
    {
        currentCorners[i].pos.x = (int)(fromCorners[i].pos.x * (1 - t) + toCorners[i].pos.x * t);
        currentCorners[i].pos.y = (int)(fromCorners[i].pos.y * (1 - t) + toCorners[i].pos.y * t);
        currentCorners[i].roundness = (int)(fromCorners[i].roundness * (1 - t) + toCorners[i].roundness * t);
    }
}

static float progress_linear = 0.0f;

int snoreTarget = -5;
int snoreCurrent = 0;
bool snoreDirection = true;
uint32_t nextSnoreTime = 0;

void dianui_update_face(uint32_t now)
{
    // Look around
    if (currentEmotion == IDLE)
    {
        int diff = targetEyePosX - currentEyePosX;

        if (abs(diff) > 0)
        {
            int step = (diff + (diff > 0 ? 2 : -2)) / 4;

            if (step == 0)
            {
                step = (diff > 0) ? 1 : -1;
            }

            currentEyePosX += step;

            leftEye->base.x = 50 + currentEyePosX;
            rightEye->base.x = 90 + currentEyePosX;
        }
        else if (now >= nextLookTime)
        {
            targetEyePosX = ((rand() % 2) - 1) * 5;
            nextLookTime = now + 1500 + (rand() % 2000);
        }
    }

    if (currentEmotion == SLEEPY)
    {
        dianui_set_visible((DianUI_BaseElement *)textEl, true);
        if (now >= nextSnoreTime)
        {
            if (snoreDirection)
            {
                snoreCurrent -= 1;

                if (snoreCurrent <= snoreTarget)
                {
                    snoreCurrent = snoreTarget;
                    snoreDirection = false;
                    nextSnoreTime = now + 1000;
                }
            }
            else
            {
                snoreCurrent += 1;

                if (snoreCurrent >= 0)
                {
                    snoreCurrent = 0;
                    snoreDirection = true;
                    nextSnoreTime = now + 3000;
                }
            }
        }

        float t = 0.0f;
        if (snoreTarget != 0)
        {
            t = (float)snoreCurrent / (float)snoreTarget;
        }

        float invT = 1.0f - t;
        float t_eased = 1.0f - (invT * invT * invT);

        int offsetY = (int)(t_eased * snoreTarget);

        leftEye->base.y = 40 + offsetY;
        rightEye->base.y = 40 + offsetY;
    }
    else
    {
        snoreCurrent = 0;
        snoreDirection = true;
        leftEye->base.y = 40;
        rightEye->base.y = 40;
        dianui_set_visible((DianUI_BaseElement *)textEl, false);
    }

    // Blink
    if (!isBlinking && now >= nextBlinkTime && currentEmotion == IDLE)
    {
        isBlinking = true;
        blinkStartTime = now;
    }

    if (isBlinking)
    {
        uint32_t elapsed = now - blinkStartTime;
        uint32_t halfDuration = BLINK_DURATION / 2;

        if (elapsed < halfDuration)
        {
            float t = (float)elapsed / (float)halfDuration;
            dianui_interpolate_emotion(leftEye, currentEmotion, BLINK, t);
            dianui_interpolate_emotion(rightEye, currentEmotion, BLINK, t);
        }
        else if (elapsed < BLINK_DURATION)
        {
            float t = (float)(elapsed - halfDuration) / (float)halfDuration;
            dianui_interpolate_emotion(leftEye, BLINK, currentEmotion, t);
            dianui_interpolate_emotion(rightEye, BLINK, currentEmotion, t);
        }
        else
        {
            isBlinking = false;
            dianui_interpolate_emotion(leftEye, currentEmotion, currentEmotion, 1.0f);
            dianui_interpolate_emotion(rightEye, currentEmotion, currentEmotion, 1.0f);
            nextBlinkTime = now + 2000 + (rand() % 3000);
        }
    }

    // Update emotion
    else
    {
        /* if (now >= nextEmotionChangeTime)
        {
            if (currentEmotion == IDLE)
            {
                targetEmotion = SLEEPY;
            }
            else if (currentEmotion == SLEEPY)
            {
                targetEmotion = IDLE;
            }
            nextEmotionChangeTime = now + 10000;
        } */

        if (currentEmotion != targetEmotion)
        {
            progress_linear += 0.25f;

            if (progress_linear > 1.0f)
            {
                progress_linear = 1.0f;
                currentEmotion = targetEmotion;
            }

            float c1 = 1.70158f;
            float c3 = c1 + 1.0f;
            float t_eased = 1.0f + c3 * (progress_linear - 1.0f) * (progress_linear - 1.0f) * (progress_linear - 1.0f) + c1 * (progress_linear - 1.0f) * (progress_linear - 1.0f);

            dianui_interpolate_emotion(leftEye, currentEmotion, targetEmotion, t_eased);
            dianui_interpolate_emotion(rightEye, currentEmotion, targetEmotion, t_eased);
        }
        else
        {
            progress_linear = 0.0f;
        }
    }

    dianui_mark_dirty((DianUI_BaseElement *)leftEye);
    dianui_mark_dirty((DianUI_BaseElement *)rightEye);
}