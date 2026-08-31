#ifndef DIANUI_TIME_H
#define DIANUI_TIME_H

#include <stdint.h>

void dianui_time_init(void);
void dianui_time_update(uint32_t current_ms);

float dianui_time_get_dt(void);
uint32_t dianui_time_get_elapsed_ms(void);

#endif // DIANUI_TIME_H