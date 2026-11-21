#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "i2c_manager.h"
#include "driver/i2c_master.h"
#include "config.h"

static const char *TAG = "I2C_MANAGER";

static i2c_master_bus_handle_t bus_handle;
static bool s_initialized = false;

void i2c_manager_init(void)
{
    i2c_master_bus_config_t i2c_mst_config = {
        .clk_source = I2C_CLK_SRC_DEFAULT,
        .i2c_port = I2C_NUM_0,
        .scl_io_num = SCL_PIN,
        .sda_io_num = SDA_PIN,
        .glitch_ignore_cnt = 7,
        .flags.enable_internal_pullup = true, 
    };

    ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_mst_config, &bus_handle));

    s_initialized = true;

    ESP_LOGI(TAG, "OK");
}

i2c_master_dev_handle_t i2c_manager_add_device(uint16_t address, uint32_t speed)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Bus not initialized. Cannot add device.");
        return NULL;
    }

    i2c_device_config_t dev_cfg = {
        .dev_addr_length = I2C_ADDR_BIT_LEN_7,
        .device_address = address,
        .scl_speed_hz = speed,
    };

    i2c_master_dev_handle_t handle;

    esp_err_t ret = i2c_master_bus_add_device(bus_handle, &dev_cfg, &handle);
    if (ret != ESP_OK) {
        ESP_LOGE(TAG, "Error adding device 0x%02X", address);
        return NULL;
    }
    
    return handle; 
}

void i2c_manager_scan(void)
{
    if (!s_initialized) {
        ESP_LOGE(TAG, "Bus not initialized. Cannot scan for devices.");
        return;
    }

    ESP_LOGI(TAG, "Scanning I2C bus for devices...");
    int devices_found = 0;

    for (uint8_t address = 1; address < 127; address++) {
        esp_err_t ret = i2c_master_probe(bus_handle, address, 10);

        if (ret == ESP_OK) {
            ESP_LOGI(TAG, " [+] Device found at address : 0x%02X", address);
            devices_found++;
        }
    }

    if (devices_found == 0) {
        ESP_LOGW(TAG, " [!] No I2C devices found. Check the wiring and power supply!");
    } else {
        ESP_LOGI(TAG, "Scan completed. %d device(s) found.", devices_found);
    }
}