#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "../i2c_manager/i2c_manager.h"
#include "display.h"
#include "esp_log.h"

static const char *TAG = "DISPLAY";

static i2c_master_dev_handle_t oled_handle;

#define VRAM_SIZE (((DISPLAY_WIDTH * DISPLAY_HEIGHT) / DISPLAY_PAGE_HEIGHT) + 1)
static uint8_t vram[VRAM_SIZE];

static const uint8_t init_cmds[] = {
    0xAE,       // Display OFF
    0xD3, 0x00, // Set display offset
    0x40,       // Set display start line
    0x20, 0x00, // Horizontal Addressing Mode
    0xA1,       // Remap segment
    0xC8,       // Scan direction
    0xA8, 0x3F, // Multiplex ratio
    0x8D, 0x14, // Charge Pump ON
    0xAF        // Display ON
};

static void display_send_cmd(uint8_t cmd)
{
    uint8_t buffer[2] = {0x00, cmd};   
    i2c_master_transmit(oled_handle, buffer, 2, 1000);
}

/* static void display_full_white(void)
{
    memset(vram, 0xFF, sizeof(vram));
} */

static void display_reset_cursor(void)
{
    display_send_cmd(0x21); // Column address
    display_send_cmd(0x00); // Start 0
    display_send_cmd(127);  // End 127
    display_send_cmd(0x22); // Page address
    display_send_cmd(0x00); // Start 0
    display_send_cmd(7);    // End 7
}

void display_set_pixel(int16_t x, int16_t y, bool color)
{
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT)
        return;

    int16_t index = (y >> 3) * DISPLAY_WIDTH + x;
    uint8_t mask = 1 << (y & 7);

    if (color)
        vram[index + 1] |= mask;
    else
        vram[index + 1] &= ~mask;
}

// Temporary hline methode (not optimized yet)
void display_set_hline(int16_t x1, int16_t x2, int16_t y, bool color)
{
    for (int16_t x = x1; x <= x2; x++)
        display_set_pixel(x, y, color);
}

// Temporary vline methode (not optimized yet)
void display_set_vline(int16_t x, int16_t y1, int16_t y2, bool color)
{
    for (int16_t y = y1; y <= y2; y++)
        display_set_pixel(x, y, color);
}

void display_clear(void)
{
    memset(vram, 0x00, sizeof(vram));
    vram[0] = 0x40;
}

void display_update(void)
{
    i2c_master_transmit(oled_handle, vram, sizeof(vram), 1000);
}

void display_set_contrast(uint8_t contrast) {
    display_send_cmd(0x81); 
    display_send_cmd(contrast); 
}

void display_init(void)
{
    oled_handle = i2c_manager_add_device(0x3C, 400000);

    for (int i = 0; i < sizeof(init_cmds); i++)
        display_send_cmd(init_cmds[i]);

    display_reset_cursor();

    display_clear();

    display_update();

    ESP_LOGI(TAG, "Initialized - OLED Display");
}