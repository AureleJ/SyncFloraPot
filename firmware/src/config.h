#ifndef CONFIG_H
#define CONFIG_H

#include "driver/gpio.h"
#include "hal/adc_types.h"

// ADC channels
#define MOISTURE_ADC_CHANNEL GPIO_NUM_3
#define LDR_ADC_CHANNEL GPIO_NUM_4

// GPIO pins
#define SDA_PIN GPIO_NUM_6
#define SCL_PIN GPIO_NUM_7 
#define MOISTURE_POWER_GPIO GPIO_NUM_2
#define PUMP_GPIO_PIN GPIO_NUM_10
#define ULTRASONIC_ECHO_PIN GPIO_NUM_20
#define ULTRASONIC_TRIG_PIN GPIO_NUM_21
#define BUTTON_GPIO_PIN GPIO_NUM_5

// Thresholds
#define LDR_NIGHT_THRESHOLD 15
#define MOISTURE_DRY_THRESHOLD 5
#define MOISTURE_HALF_DRY_THRESHOLD 15
#define WATER_LEVEL_MIN_THRESHOLD 4.5f
#define WATER_LEVEL_MAX_THRESHOLD 1.5f

// Timings
#define MONITORING_INTERVAL_MS 1000 // Interval between sensor readings

#endif // CONFIG_H