// Faire l'interpolation entre des emotions
// Faire le regard dans differents directions
// Faire le clignement des yeux

#include "dianui_face.h"
#include "dianui/core/dianui_types.h"
#include "dianui/dianui.h"
#include "math.h"

DianUI_PolygonElement *leftEye = 0;
DianUI_PolygonElement *rightEye = 0;
DianUI_PolygonElement *mouse = 0;

DianUI_Corner currentLeftEyeCorners[4];
DianUI_Corner currentRightEyeCorners[4];

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
    {.pos = {.x = 20, .y = 0}, .roundness = 8},
    {.pos = {.x = 20, .y = 30}, .roundness = 8},
    {.pos = {.x = 0, .y = 30}, .roundness = 8},
};

// Happy
DianUI_Corner happyEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 8}, .roundness = 0},
    {.pos = {.x = 0, .y = 8}, .roundness = 0}};

// Sleepy
DianUI_Corner sleepyEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 3},
    {.pos = {.x = 45, .y = 0}, .roundness = 3},
    {.pos = {.x = 45, .y = 5}, .roundness = 3},
    {.pos = {.x = 0, .y = 5}, .roundness = 3}};

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

void dianui_face_init()
{
    for (int i = 0; i < 4; i++)
    {
        currentLeftEyeCorners[i] = normalEye[i];
        currentRightEyeCorners[i] = normalEye[i];
    }

    leftEye = dianui_create_polygon(
        (DianUI_Point){.x = 45, .y = 40},
        currentLeftEyeCorners,
        DIANUI_CENTER,
        DIANUI_CENTER,
        DIANUI_WHITE);

    rightEye = dianui_create_polygon(
        (DianUI_Point){.x = 95, .y = 40},
        currentRightEyeCorners,
        DIANUI_CENTER,
        DIANUI_CENTER,
        DIANUI_WHITE);
}

int targetEyePosX = 0;
int currentLeftEyePosX = 0;
int currentRightEyePosX = 0;
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

// Implémentation brute a revoir
void dianui_interpolate_emotion(Emotion from, Emotion to, float t)
{
    DianUI_Corner fromCorners[4];
    DianUI_Corner toCorners[4];

    switch (from)
    {
    case IDLE:
        for (int i = 0; i < 4; i++)
        {
            fromCorners[i] = normalEye[i];
        }
        break;
    case HAPPY:
        for (int i = 0; i < 4; i++)
        {
            fromCorners[i] = happyEye[i];
        }
        break;
    case BLINK:
        for (int i = 0; i < 4; i++)
        {
            fromCorners[i] = blinkEye[i];
        }
        break;
    case SLEEPY:
        for (int i = 0; i < 4; i++)
        {
            fromCorners[i] = sleepyEye[i];
        }
        break;
    }

    switch (to)
    {
    case IDLE:
        for (int i = 0; i < 4; i++)
        {
            toCorners[i] = normalEye[i];
        }
        break;
    case HAPPY:
        for (int i = 0; i < 4; i++)
        {
            toCorners[i] = happyEye[i];
        }
        break;
    case BLINK:
        for (int i = 0; i < 4; i++)
        {
            toCorners[i] = blinkEye[i];
        }
        break;
    case SLEEPY:
        for (int i = 0; i < 4; i++)
        {
            toCorners[i] = sleepyEye[i];
        }
        break;
    }

    for (int i = 0; i < 4; i++)
    {
        currentLeftEyeCorners[i].pos.x = (int)(fromCorners[i].pos.x * (1 - t) + toCorners[i].pos.x * t);
        currentLeftEyeCorners[i].pos.y = (int)(fromCorners[i].pos.y * (1 - t) + toCorners[i].pos.y * t);
        currentLeftEyeCorners[i].roundness = (int)(fromCorners[i].roundness * (1 - t) + toCorners[i].roundness * t);

        currentRightEyeCorners[i].pos.x = (int)(fromCorners[i].pos.x * (1 - t) + toCorners[i].pos.x * t);
        currentRightEyeCorners[i].pos.y = (int)(fromCorners[i].pos.y * (1 - t) + toCorners[i].pos.y * t);
        currentRightEyeCorners[i].roundness = (int)(fromCorners[i].roundness * (1 - t) + toCorners[i].roundness * t);
    }

    leftEye->corners[0] = currentLeftEyeCorners[0];
    leftEye->corners[1] = currentLeftEyeCorners[1];
    leftEye->corners[2] = currentLeftEyeCorners[2];
    leftEye->corners[3] = currentLeftEyeCorners[3];

    rightEye->corners[0] = currentRightEyeCorners[0];
    rightEye->corners[1] = currentRightEyeCorners[1];
    rightEye->corners[2] = currentRightEyeCorners[2];
    rightEye->corners[3] = currentRightEyeCorners[3];

    leftEye->base.dirty = true;
    rightEye->base.dirty = true;
}

static float progress_linear = 0.0f;

void dianui_update_face(uint32_t now)
{
    // Look around
    if (currentEmotion == IDLE)
    {
        int diffLeft = targetEyePosX - currentLeftEyePosX;
        int diffRight = targetEyePosX - currentRightEyePosX;

        if (abs(diffLeft) > 0 || abs(diffRight) > 0)
        {
            int stepLeft = (diffLeft + (diffLeft > 0 ? 2 : -2)) / 4;
            int stepRight = (diffRight + (diffRight > 0 ? 2 : -2)) / 4;

            if (stepLeft == 0 && diffLeft != 0)
            {
                stepLeft = (diffLeft > 0) ? 1 : -1;
            }

            if (stepRight == 0 && diffRight != 0)
            {
                stepRight = (diffRight > 0) ? 1 : -1;
            }

            currentLeftEyePosX += stepLeft;
            currentRightEyePosX += stepRight;

            leftEye->base.x = 45 + currentLeftEyePosX;
            rightEye->base.x = 95 + currentRightEyePosX;

            leftEye->base.dirty = true;
            rightEye->base.dirty = true;
        }
        else if (now >= nextLookTime)
        {
            targetEyePosX = ((rand() % 2) - 1) * 5;
            nextLookTime = now + 1500 + (rand() % 2000);
        }
    }

    if (currentEmotion == SLEEPY)
    {
        
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
            dianui_interpolate_emotion(currentEmotion, BLINK, t);
        }
        else if (elapsed < BLINK_DURATION)
        {
            float t = (float)(elapsed - halfDuration) / (float)halfDuration;
            dianui_interpolate_emotion(BLINK, currentEmotion, t);
        }
        else
        {
            isBlinking = false;
            dianui_interpolate_emotion(currentEmotion, currentEmotion, 1.0f);
            nextBlinkTime = now + 2000 + (rand() % 3000);
        }
    }

    // Update emotion
    else
    {
        if (now >= nextEmotionChangeTime)
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
        }

        if (currentEmotion != targetEmotion)
        {
            progress_linear += 0.25f;

            if (progress_linear > 1.0f)
            {
                progress_linear = 1.0f;
                currentEmotion = targetEmotion;
            }

            float c1 = 1.70158;
            float c3 = c1 + 1;
            float t_eased = 1 + c3 * (progress_linear - 1) * (progress_linear - 1) * (progress_linear - 1) + c1 * (progress_linear - 1) * (progress_linear - 1);

            dianui_interpolate_emotion(currentEmotion, targetEmotion, t_eased);
        }
        else
        {
            progress_linear = 0.0f;
        }
    }
}