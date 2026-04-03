/**
  ******************************************************************************
  * @file    app.h
  * @brief   RTOS应用层公共接口 - STM32F103C8T6 用户模板
  * @date    2026-04-02
  ******************************************************************************
  */

#ifndef APP_H
#define APP_H

#include <stddef.h>

#include "app_config.h"
#include "FreeRTOS.h"
#include "task.h"
#include "queue.h"
#include "semphr.h"

extern SemaphoreHandle_t g_mutex_shared;
extern QueueHandle_t     g_queue_msg;
extern SemaphoreHandle_t g_sem_event;

void App_Init(void);

#endif /* APP_H */
