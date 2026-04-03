/**
  ******************************************************************************
  * @file    app_config.h
  * @brief   RTOS应用层配置文件 - STM32F103C8T6 用户模板
  * @date    2026-04-02
  ******************************************************************************
  */

#ifndef APP_CONFIG_H
#define APP_CONFIG_H

#include "stm32f10x.h"

#define TASK_PRIORITY_LOW           1
#define TASK_PRIORITY_NORMAL        2
#define TASK_PRIORITY_HIGH          3
#define TASK_PRIORITY_REALTIME      4

#define ENABLE_TASK1                1
#define ENABLE_TASK2                1
#define ENABLE_TASK3                1

#define APP_ENABLE_PERIPHERAL_SELFTEST  1
#define APP_BOOT_BANNER_REPEAT          3U
#define APP_BOOT_BANNER_DELAY_MS        200U

#define TASK_STACK_SIZE_TASK1       192
#define TASK_STACK_SIZE_TASK2       256
#define TASK_STACK_SIZE_TASK3       256

#define TASK_PERIOD_TASK1           1000U
#define TASK_PERIOD_TASK2           1000U
#define TASK_PERIOD_TASK3           1000U

#define TASK1_PRIORITY              TASK_PRIORITY_NORMAL
#define TASK2_PRIORITY              TASK_PRIORITY_NORMAL
#define TASK3_PRIORITY              TASK_PRIORITY_HIGH

#define QUEUE_SIZE_MSG              8U

typedef struct
{
    uint32_t id;
    uint32_t value;
} AppMessage_t;

#endif /* APP_CONFIG_H */
