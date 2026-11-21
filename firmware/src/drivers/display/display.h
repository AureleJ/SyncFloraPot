#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 64
#define DISPLAY_PAGE_HEIGHT 8

void display_init(void);

void display_set_pixel(int16_t x, int16_t y, bool color);

void display_set_hline(int16_t x1, int16_t x2, int16_t y, bool color);

void display_set_vline(int16_t x, int16_t y1, int16_t y2, bool color);

void display_clear(void);

void display_update(void);

void display_set_contrast(uint8_t contrast);

#endif // DISPLAY_H