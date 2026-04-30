#ifndef TASK_MONITORING_H
#define TASK_MONITORING_H

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"

void srv_monitoring_task(void *pvParameters);

#endif // TASK_MONITORING_H