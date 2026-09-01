#include "dianui_face.h"
#include "../core/dianui_time.h"
#include "../core/dianui_types.h"
#include "../animation/dianui_easing.h"
#include "../dianui.h"
#include <math.h>
#include <stdlib.h>

// Blink
static const DianUI_Corner blinkEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 4},
    {.pos = {.x = 40, .y = 0}, .roundness = 4},
    {.pos = {.x = 40, .y = 3}, .roundness = 4},
    {.pos = {.x = 0, .y = 3}, .roundness = 4},
};

// Idle
static const DianUI_Corner normalEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 30}, .roundness = 8},
    {.pos = {.x = 0, .y = 30}, .roundness = 8},
};

// Happy
static const DianUI_Corner happyEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 0}, .roundness = 6},
    {.pos = {.x = 20, .y = 8}, .roundness = 0},
    {.pos = {.x = 0, .y = 8}, .roundness = 0}};

// Sleepy
static const DianUI_Corner sleepyEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 3},
    {.pos = {.x = 35, .y = 0}, .roundness = 3},
    {.pos = {.x = 35, .y = 5}, .roundness = 3},
    {.pos = {.x = 0, .y = 5}, .roundness = 3}};

// Thirsty
static const DianUI_Corner thirstyEyeLeft[4] = {
    {.pos = {.x = 0, .y = 15}, .roundness = 4},
    {.pos = {.x = 30, .y = 0}, .roundness = 6},
    {.pos = {.x = 30, .y = 30}, .roundness = 6},
    {.pos = {.x = 0, .y = 30}, .roundness = 4}};

static const DianUI_Corner thirstyEyeRight[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 30, .y = 15}, .roundness = 4},
    {.pos = {.x = 30, .y = 30}, .roundness = 4},
    {.pos = {.x = 0, .y = 30}, .roundness = 6}};

// Almost dead
static const DianUI_Corner almostDeadEyeLeft[4] = {
    {.pos = {.x = 0, .y = 15}, .roundness = 4},
    {.pos = {.x = 30, .y = 0}, .roundness = 6},
    {.pos = {.x = 30, .y = 20}, .roundness = 6},
    {.pos = {.x = 0, .y = 20}, .roundness = 4}};

static const DianUI_Corner almostDeadEyeRight[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 6},
    {.pos = {.x = 30, .y = 15}, .roundness = 4},
    {.pos = {.x = 30, .y = 30}, .roundness = 4},
    {.pos = {.x = 0, .y = 30}, .roundness = 6}};

// Wink
static const DianUI_Corner winkLeftEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 3},
    {.pos = {.x = 30, .y = 0}, .roundness = 3},
    {.pos = {.x = 30, .y = 10}, .roundness = 3},
    {.pos = {.x = 0, .y = 10}, .roundness = 3},
};

static const DianUI_Corner winkRightEye[4] = {
    {.pos = {.x = 0, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 0}, .roundness = 8},
    {.pos = {.x = 30, .y = 30}, .roundness = 8},
    {.pos = {.x = 0, .y = 30}, .roundness = 8},
};

static DianUI_TextElement *textEl = 0;

static DianUI_PolygonElement *leftEye = 0;
static DianUI_PolygonElement *rightEye = 0;

static Emotion currentEmotion = IDLE;
static Emotion targetEmotion = IDLE;

static float lookProgress = 1.0f;
static float nextLookTimer = 0.0f;
static int startEyePosX = 0;
static int targetEyePosX = 0;
static int startEyePosY = 0;
static int targetEyePosY = 0;

static float nextBlinkTimer = 0.0f;
static float blinkProgress = 0.0f;

static float snoreProgress = 1.0f;
static float nextSnoreTimer = 0;

static float emotionsProgress = 1.0f;

static int offset_look_x = 0;
static int offset_look_y = 0;
static int offset_snore_y = 0;

static const int EYE_LEFT_BASE_X = 45;
static const int EYE_RIGHT_BASE_X = 85;
static const int EYE_COMMON_BASE_Y = 35;

static const DianUI_Corner *EMOTION_REGISTRY_LEFT[] = {
    [IDLE] = normalEye,
    [HAPPY] = happyEye,
    [BLINK] = blinkEye,
    [SLEEPY] = sleepyEye,
    [THIRSTY] = thirstyEyeLeft,
    [ALMOST_DEAD] = almostDeadEyeLeft,
    [WINK] = winkLeftEye};

static const DianUI_Corner *EMOTION_REGISTRY_RIGHT[] = {
    [IDLE] = normalEye,
    [HAPPY] = happyEye,
    [BLINK] = blinkEye,
    [SLEEPY] = sleepyEye,
    [THIRSTY] = thirstyEyeRight,
    [ALMOST_DEAD] = almostDeadEyeRight,
    [WINK] = winkRightEye};

static inline int lerp_int(int start, int target, float t)
{
    return (int)roundf((float)start + ((float)(target - start) * t));
}

void dianui_face_init()
{
    leftEye = dianui_create_polygon(
        (DianUI_Point){.x = EYE_LEFT_BASE_X, .y = EYE_COMMON_BASE_Y},
        normalEye,
        DIANUI_CENTER,
        DIANUI_CENTER,
        DIANUI_WHITE);

    rightEye = dianui_create_polygon(
        (DianUI_Point){.x = EYE_RIGHT_BASE_X, .y = EYE_COMMON_BASE_Y},
        normalEye,
        DIANUI_CENTER,
        DIANUI_CENTER,
        DIANUI_WHITE);

    textEl = dianui_create_text(0, 10, 128, 64, DIANUI_RIGHT, DIANUI_TOP, "zzz", DIANUI_WHITE);
    dianui_set_visible((DianUI_BaseElement *)textEl, false);
}

void dianui_set_emotion(Emotion emotion)
{
    targetEmotion = emotion;
}

void dianui_interpolate_emotion(DianUI_PolygonElement *polygon, const DianUI_Corner *fromCorners, const DianUI_Corner *toCorners, float t)
{
    DianUI_Corner *currentCorners = polygon->corners;

    for (int i = 0; i < 4; i++)
    {
        currentCorners[i].pos.x = lerp_int(fromCorners[i].pos.x, toCorners[i].pos.x, t);
        currentCorners[i].pos.y = lerp_int(fromCorners[i].pos.y, toCorners[i].pos.y, t);
        currentCorners[i].roundness = lerp_int(fromCorners[i].roundness, toCorners[i].roundness, t);
    }
}

static void dianui_update_look(float dt)
{
    const float LOOK_ANIMATION_SPEED = 1.0f / 0.25f;

    if (lookProgress < 1.0f)
    {
        lookProgress += LOOK_ANIMATION_SPEED * dt;

        if (lookProgress >= 1.0f)
        {
            lookProgress = 1.0f;
        }

        float t_eased = dianui_apply_easing(lookProgress, DIANUI_EASE_OUT_QUAD);

        offset_look_x = lerp_int(startEyePosX, targetEyePosX, t_eased); // X axis
        offset_look_y = lerp_int(startEyePosY, targetEyePosY, t_eased); // Y axis
    }
    else
    {
        nextLookTimer -= dt;

        if (nextLookTimer <= 0.0f)
        {
            nextLookTimer = 2.0f + ((float)(rand() % 3000) / 1000.0f);

            targetEyePosX = ((rand() % 3) - 1) * 8;
            targetEyePosY = ((rand() % 3) - 1) * 3;

            startEyePosX = offset_look_x;
            startEyePosY = offset_look_y;

            lookProgress = 0.0f;
        }
    }
}

static void dianui_update_blink(float dt)
{
    const float BLINK_ANIMATION_SPEED = 1.0f / 0.16f;

    if (blinkProgress < 1.0f)
    {
        blinkProgress += BLINK_ANIMATION_SPEED * dt;

        if (blinkProgress >= 1.0f)
        {
            blinkProgress = 1.0f;
            nextBlinkTimer = 2.0f + ((float)(rand() % 2000) / 1000.0f);
        }

        float t_eased = 4.0f * blinkProgress * (1.0f - blinkProgress);

        dianui_interpolate_emotion(leftEye, EMOTION_REGISTRY_LEFT[currentEmotion], EMOTION_REGISTRY_LEFT[BLINK], t_eased);
        dianui_interpolate_emotion(rightEye, EMOTION_REGISTRY_RIGHT[currentEmotion], EMOTION_REGISTRY_RIGHT[BLINK], t_eased);
    }
    else
    {
        nextBlinkTimer -= dt;

        if (nextBlinkTimer <= 0.0f)
            blinkProgress = 0.0f;
    }
}

static void dianui_update_snore(float dt)
{
    const float SNORE_ANIMATION_SPEED = 1.0f / 1.5f;

    if (snoreProgress < 1.0f)
    {
        snoreProgress += SNORE_ANIMATION_SPEED * dt;

        if (snoreProgress >= 1.0f)
        {
            snoreProgress = 1.0f;
            nextSnoreTimer = 3.0f;
        }

        float t_eased = 5.0f * snoreProgress * (1.0f - snoreProgress);

        if (t_eased > 1.0f)
        {
            t_eased = 1.0f;
        }

        offset_snore_y = lerp_int(5, -5, t_eased);
    }
    else
    {
        nextSnoreTimer -= dt;

        if (nextSnoreTimer <= 0.0f)
            snoreProgress = 0.0f;
    }
}

static void dianui_update_emotion(float dt)
{
    const float EMOTION_CHANGE_SPEED = 1.0f / 0.3f;

    if (currentEmotion != targetEmotion)
    {
        emotionsProgress += EMOTION_CHANGE_SPEED * dt;

        if (emotionsProgress >= 1.0f)
        {
            emotionsProgress = 1.0f;
            currentEmotion = targetEmotion;
        }

        float t_eased = dianui_apply_easing(emotionsProgress, DIANUI_EASE_OUT_BACK);

        dianui_interpolate_emotion(leftEye, EMOTION_REGISTRY_LEFT[currentEmotion], EMOTION_REGISTRY_LEFT[targetEmotion], t_eased);
        dianui_interpolate_emotion(rightEye, EMOTION_REGISTRY_RIGHT[currentEmotion], EMOTION_REGISTRY_RIGHT[targetEmotion], t_eased);
    }
    else
    {
        emotionsProgress = 0.0f;
    }
}

void dianui_update_face()
{
    float dt = dianui_time_get_dt();

    if (currentEmotion == SLEEPY)
    {
        offset_look_x = 0;
        offset_look_y = 0;
        lookProgress = 1.0f;

        dianui_set_visible((DianUI_BaseElement *)textEl, true);

        // SNORE
        dianui_update_snore(dt);
    }
    else
    {
        offset_snore_y = 0;
        snoreProgress = 1.0f;

        dianui_set_visible((DianUI_BaseElement *)textEl, false);

        // LOOK AROUND
        dianui_update_look(dt);

        // BLINK
        if (currentEmotion == targetEmotion)
            dianui_update_blink(dt);
    }

    // UPDATE EMOTION
    dianui_update_emotion(dt);

    int finalLeftEyeX = EYE_LEFT_BASE_X + offset_look_x;
    int finalRightEyeX = EYE_RIGHT_BASE_X + offset_look_x;

    int finalCommonEyeY = EYE_COMMON_BASE_Y + offset_look_y + offset_snore_y;

    leftEye->base.x = finalLeftEyeX;
    rightEye->base.x = finalRightEyeX;

    leftEye->base.y = finalCommonEyeY;
    rightEye->base.y = finalCommonEyeY;

    dianui_mark_dirty((DianUI_BaseElement *)leftEye);
    dianui_mark_dirty((DianUI_BaseElement *)rightEye);
}