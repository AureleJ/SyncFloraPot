#include "dianui_time.h"

typedef struct
{
    uint32_t last_ms;
    uint32_t elapsed_ms;
    float dt;
} DianUI_TimeState;

static DianUI_TimeState g_time_state = {0};

void dianui_time_init(void)
{
    g_time_state.last_ms = 0;
    g_time_state.elapsed_ms = 0;
    g_time_state.dt = 0.0f;
}

void dianui_time_update(uint32_t current_ms)
{
    if (g_time_state.last_ms == 0)
    {
        g_time_state.last_ms = current_ms;
        return;
    }

    g_time_state.elapsed_ms = current_ms - g_time_state.last_ms;
    g_time_state.dt = (float)g_time_state.elapsed_ms / 1000.0f; 
    g_time_state.last_ms = current_ms;

    if (g_time_state.dt > 0.1f)
    {
        g_time_state.dt = 0.1f;
    }
}

float dianui_time_get_dt(void)
{
    return g_time_state.dt;
}

uint32_t dianui_time_get_elapsed_ms(void)
{
    return g_time_state.elapsed_ms;
}