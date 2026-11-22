#ifndef PLANT_DATA_H
#define PLANT_DATA_H

#include <stdbool.h>

typedef struct
{
    int soil_moisture; // Soil moisture percentage (0-100)
    int light;         // Light intensity percentage (0-100)
    int water_level;   // Water level percentage (0-100)
    bool pump_state;   // Pump state (true for ON, false for OFF)
} plant_data_t;

#endif // PLANT_DATA_H
