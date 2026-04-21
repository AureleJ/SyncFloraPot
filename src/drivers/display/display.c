#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/i2c_master.h"
#include "../i2c_manager/i2c_manager.h"
#include "display.h"
#include "esp_log.h"

static const char *TAG = "DISPLAY";

static i2c_master_dev_handle_t oled_handle;

static uint8_t vram[(DISPLAY_WIDTH * DISPLAY_HEIGHT) / DISPLAY_PAGE_HEIGHT];

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

void display_set_pixel(int x, int y, bool color)
{
    if (x < 0 || x >= DISPLAY_WIDTH || y < 0 || y >= DISPLAY_HEIGHT)
        return;

    int index = (y / DISPLAY_PAGE_HEIGHT) * DISPLAY_WIDTH + x;

    if (color)
        vram[index] |= (1 << (y % DISPLAY_PAGE_HEIGHT));
    else
        vram[index] &= ~(1 << (y % DISPLAY_PAGE_HEIGHT));
}

void display_clear(void)
{
    memset(vram, 0x00, sizeof(vram));
}

void display_update(void)
{
    uint8_t buffer[1 + sizeof(vram)];
    buffer[0] = 0x40;
    memcpy(&buffer[1], vram, sizeof(vram));

    i2c_master_transmit(oled_handle, buffer, sizeof(buffer), 1000);
}