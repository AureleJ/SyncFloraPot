#ifndef SENSOR_STATE_H
#define SENSOR_STATE_H

#include "plant_data.h"

void sensor_state_init(void);

void sensor_state_update_telemetry(int light, int water_level, int soil_moisture);

void sensor_state_update_pump(bool pump_on);

bool sensor_state_is_pump_on(void);

plant_data_t sensor_state_get_copy(void);  

#endif // SENSOR_STATE_H