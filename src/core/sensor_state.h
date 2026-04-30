#ifndef SENSOR_STATE_H
#define SENSOR_STATE_H

#include "plant_data.h"

void sensor_state_init(void);

void sensor_state_update_light(int light_pct);

void sensor_state_update_water(float water_cm);

void sensor_state_update_soil(int soil_pct);

void sensor_state_update_pump(bool pump_on);

plant_data_t sensor_state_get_copy(void);  

#endif // SENSOR_STATE_H