#ifndef I2C_MANAGER_H
#define I2C_MANAGER_H

#include "driver/i2c_master.h"

void i2c_manager_init(void);

i2c_master_dev_handle_t i2c_manager_add_device(uint16_t address, uint32_t speed);

void i2c_manager_scan(void);

#endif // I2C_MANAGER_H