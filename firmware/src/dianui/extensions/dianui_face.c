#include "dianui_face.h"
#include "dianui/core/dianui_types.h"
#include "dianui/dianui.h"
#include "math.h"

DianUI_PolygonElement *leftEye = 0;
DianUI_PolygonElement *rightEye = 0;

Emotion currentEmotion = IDLE;
Emotion targetEmotion = IDLE;

// Blink
const DianUI_Corner blinkEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 4},
    {.pos = {.x = 40, .y = 0}, .roundness = 4},
    {.pos = {.x = 40, .y = 3}, .roundness = 4},
    {.pos = {.x = 0, .y = 3}, .roundness = 4},
};

// Idle
const DianUI_Corner normalEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 30}, .roundness = 8},
    {.pos = {.x = 0, .y = 30}, .roundness = 8},
};

// Happy
const DianUI_Corner happyEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 8}, .roundness = 0},
    {.pos = {.x = 0, .y = 8}, .roundness = 0}};

// Sleepy
const DianUI_Corner sleepyEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 3},
    {.pos = {.x = 35, .y = 0}, .roundness = 3},
    {.pos = {.x = 35, .y = 5}, .roundness = 3},
    {.pos = {.x = 0, .y = 5}, .roundness = 3}};

// Thirsty
const DianUI_Corner thirstyEyeLeft[4] = {
    {.pos = {.x = 0, .y = 15}, .roundness = 4},
    {.pos = {.x = 30, .y = 0}, .roundness = 6},
    {.pos = {.x = 30, .y = 30}, .roundness = 6},
    {.pos = {.x = 0, .y = 30}, .roundness = 4}};

const DianUI_Corner thirstyEyeRight[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 30, .y = 15}, .roundness = 4},
    {.pos = {.x = 30, .y = 30}, .roundness = 4},
    {.pos = {.x = 0, .y = 30}, .roundness = 6}};

// Almost dead
const DianUI_Corner almostDeadEyeLeft[4] = {
    {.pos = {.x = 0, .y = 15}, .roundness = 4},
    {.pos = {.x = 30, .y = 0}, .roundness = 6},
    {.pos = {.x = 30, .y = 20}, .roundness = 6},
    {.pos = {.x = 0, .y = 20}, .roundness = 4}};

const DianUI_Corner almostDeadEyeRight[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 30, .y = 15}, .roundness = 4},
    {.pos = {.x = 30, .y = 30}, .roundness = 4},
    {.pos = {.x = 0, .y = 30}, .roundness = 6}};

// Wink
const DianUI_Corner winkLeftEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 3},
    {.pos = {.x = 30, .y = 0}, .roundness = 3},
    {.pos = {.x = 30, .y = 10}, .roundness = 3},
    {.pos = {.x = 0, .y = 10}, .roundness = 3},
};

const DianUI_Corner winkRightEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 30}, .roundness = 8},
    {.pos = {.x = 0, .y = 30}, .roundness = 8},
};

DianUI_TextElement *textEl = 0;

int targetEyePosX = 0;
int currentEyePosX = 0;
int startEyePosX = 0;
float progressEyePosX = 1.0f;

int targetEyePosY = 0;
int currentEyePosY = 0;
int startEyePosY = 0;
float progressEyePosY = 1.0f;

uint32_t nextLookTime = 0;

uint32_t nextBlinkTime = 0;
bool isBlinking = false;
uint32_t blinkStartTime = 0;
#define BLINK_DURATION 160

uint32_t nextEmotionChangeTime = 0;

static float progress_linear = 0.0f;

int snoreTarget = -5;
int snoreCurrent = 0;
bool snoreDirection = true;
uint32_t nextSnoreTime = 0;

void dianui_face_init()
{
    leftEye = dianui_create_polygon(
        (DianUI_Point){.x = 50, .y = 30},
        normalEye,
        DIANUI_CENTER,
        DIANUI_CENTER,
        DIANUI_WHITE);

    rightEye = dianui_create_polygon(
        (DianUI_Point){.x = 90, .y = 30},
        normalEye,
        DIANUI_CENTER,
        DIANUI_CENTER,
        DIANUI_WHITE);

    textEl = dianui_create_text(10, 10, 128, 64, DIANUI_LEFT, DIANUI_TOP, "Zzz", DIANUI_WHITE);
    dianui_set_visible((DianUI_BaseElement *)textEl, false);
}

void dianui_set_emotion(Emotion emotion)
{
    targetEmotion = emotion;
}

const DianUI_Corner *EMOTION_REGISTRY_LEFT[] = {
    [IDLE] = normalEye,
    [HAPPY] = happyEye,
    [BLINK] = blinkEye,
    [SLEEPY] = sleepyEye,
    [THIRSTY] = thirstyEyeLeft,
    [ALMOST_DEAD] = almostDeadEyeLeft,
    [WINK] = winkLeftEye};

const DianUI_Corner *EMOTION_REGISTRY_RIGHT[] = {
    [IDLE] = normalEye,
    [HAPPY] = happyEye,
    [BLINK] = blinkEye,
    [SLEEPY] = sleepyEye,
    [THIRSTY] = thirstyEyeRight,
    [ALMOST_DEAD] = almostDeadEyeRight,
    [WINK] = winkRightEye};

void dianui_interpolate_emotion(DianUI_PolygonElement *polygon, const DianUI_Corner *fromCorners, const DianUI_Corner *toCorners, float t)
{
    DianUI_Corner *currentCorners = polygon->corners;

    for (int i = 0; i < 4; i++)
    {
        currentCorners[i].pos.x = (int)(fromCorners[i].pos.x * (1 - t) + toCorners[i].pos.x * t);
        currentCorners[i].pos.y = (int)(fromCorners[i].pos.y * (1 - t) + toCorners[i].pos.y * t);
        currentCorners[i].roundness = (int)(fromCorners[i].roundness * (1 - t) + toCorners[i].roundness * t);
    }
}

void dianui_update_face(uint32_t now)
{
    // Look around
    if (currentEmotion == IDLE || currentEmotion == HAPPY || currentEmotion == THIRSTY || currentEmotion == ALMOST_DEAD)
    {
        // X axis
        if (progressEyePosX < 1.0f)
        {
            progressEyePosX += 0.25f;

            if (progressEyePosX >= 1.0f)
            {
                progressEyePosX = 1.0f;
            }

            float invT = 1.0f - progressEyePosX;
            float t_eased = 1.0f - (invT * invT * invT);

            currentEyePosX = startEyePosX + (int)(t_eased * (targetEyePosX - startEyePosX));

            leftEye->base.x = 50 + currentEyePosX;
            rightEye->base.x = 90 + currentEyePosX;
        }
        else if (now >= nextLookTime)
        {
            startEyePosX = currentEyePosX;
            progressEyePosX = 0.0f;
            targetEyePosX = ((rand() % 2) - 1) * 6;
        }

        // Y axis
        if (progressEyePosY < 1.0f)
        {
            progressEyePosY += 0.25f;

            if (progressEyePosY >= 1.0f)
            {
                progressEyePosY = 1.0f;
            }

            float invT = 1.0f - progressEyePosY;
            float t_eased = 1.0f - (invT * invT * invT);

            currentEyePosY = startEyePosY + (int)(t_eased * (targetEyePosY - startEyePosY));

            leftEye->base.y = 30 + currentEyePosY;
            rightEye->base.y = 30 + currentEyePosY;
        }
        else if (now >= nextLookTime)
        {
            startEyePosY = currentEyePosY;
            progressEyePosY = 0.0f;
            targetEyePosY = ((rand() % 2) - 1) * 3;
        }

        if (now >= nextLookTime)
        {
            nextLookTime = now + 4000 + (rand() % 3000);
        }
    }

    // Snore animation
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

        leftEye->base.y = 30 + offsetY;
        rightEye->base.y = 30 + offsetY;
    }
    else
    {
        snoreCurrent = 0;
        snoreDirection = true;
        // leftEye->base.y = 30;
        // rightEye->base.y = 30;
        dianui_set_visible((DianUI_BaseElement *)textEl, false);
    }

    // Blink
    if (!isBlinking && now >= nextBlinkTime && (currentEmotion == IDLE || currentEmotion == THIRSTY))
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
            dianui_interpolate_emotion(leftEye, EMOTION_REGISTRY_LEFT[currentEmotion], EMOTION_REGISTRY_LEFT[BLINK], t);
            dianui_interpolate_emotion(rightEye, EMOTION_REGISTRY_RIGHT[currentEmotion], EMOTION_REGISTRY_RIGHT[BLINK], t);
        }
        else if (elapsed < BLINK_DURATION)
        {
            float t = (float)(elapsed - halfDuration) / (float)halfDuration;
            dianui_interpolate_emotion(leftEye, EMOTION_REGISTRY_LEFT[BLINK], EMOTION_REGISTRY_LEFT[currentEmotion], t);
            dianui_interpolate_emotion(rightEye, EMOTION_REGISTRY_RIGHT[BLINK], EMOTION_REGISTRY_RIGHT[currentEmotion], t);
        }
        else
        {
            isBlinking = false;
            dianui_interpolate_emotion(leftEye, EMOTION_REGISTRY_LEFT[currentEmotion], EMOTION_REGISTRY_LEFT[currentEmotion], 1.0f);
            dianui_interpolate_emotion(rightEye, EMOTION_REGISTRY_RIGHT[currentEmotion], EMOTION_REGISTRY_RIGHT[currentEmotion], 1.0f);
            nextBlinkTime = now + 4000 + (rand() % 3000);
        }
    }

    // Update emotion
    else
    {
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
            float invT = 1.0f - progress_linear;
            float t_eased = 1.0f + c3 * invT * invT * invT + c1 * invT * invT;

            dianui_interpolate_emotion(leftEye, EMOTION_REGISTRY_LEFT[currentEmotion], EMOTION_REGISTRY_LEFT[targetEmotion], t_eased);
            dianui_interpolate_emotion(rightEye, EMOTION_REGISTRY_RIGHT[currentEmotion], EMOTION_REGISTRY_RIGHT[targetEmotion], t_eased);
        }
        else
        {
            progress_linear = 0.0f;
        }
    }

    dianui_mark_dirty((DianUI_BaseElement *)leftEye);
    dianui_mark_dirty((DianUI_BaseElement *)rightEye);
}