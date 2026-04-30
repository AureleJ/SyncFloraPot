#ifndef PLANT_DATA_H
#define PLANT_DATA_H

#include <stdbool.h>

typedef struct {
    int soil_moisture;
    int light;
    float water_level;
    bool pump_state;
} plant_data_t;

#endif // PLANT_DATA_H
