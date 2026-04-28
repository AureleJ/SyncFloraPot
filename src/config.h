#ifndef CONFIG_H
#define CONFIG_H

#include "driver/gpio.h"
#include "hal/adc_types.h"

// ADC channels
#define MOISTURE_ADC_CHANNEL GPIO_NUM_2
#define LDR_ADC_CHANNEL GPIO_NUM_3

// GPIO pins
#define DHT11_GPIO_PIN GPIO_NUM_4
#define MOISTURE_POWER_GPIO GPIO_NUM_5
#define OLED_SDA_PIN GPIO_NUM_6
#define OLED_SCL_PIN GPIO_NUM_7 
#define PUMP_GPIO_PIN GPIO_NUM_10
#define ULTRASONIC_ECHO_PIN GPIO_NUM_20
#define ULTRASONIC_TRIG_PIN GPIO_NUM_21

// Thresholds
#define MOISTURE_THRESHOLD 10 // Percentage below which we consider the soil "dry"
#define WATER_LEVEL_THRESHOLD 20.0f // Distance in cm below which we consider the water level "low"

// Timings
#define MONITORING_INTERVAL_MS 10000 // Interval between sensor readings

#endif // CONFIG_H