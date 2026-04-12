#ifndef CONFIG_H
#define CONFIG_H

#include "driver/gpio.h"
#include "driver/adc.h"

// GPIO pins
#define DHT11_GPIO_PIN GPIO_NUM_4
#define PUMP_GPIO_PIN GPIO_NUM_20
#define ULTRASONIC_TRIG_PIN GPIO_NUM_2
#define ULTRASONIC_ECHO_PIN GPIO_NUM_3

// ADC channels
#define LDR_ADC_CHANNEL ADC_CHANNEL_2
#define MOISTURE_ADC_CHANNEL ADC_CHANNEL_3

// Thresholds
#define MOISTURE_THRESHOLD 30
#define LIGHT_THRESHOLD 20

// Timings
#define PUMP_ON_MS 5000
#define PUMP_COOLDOWN_MS 30000

#endif // CONFIG_H