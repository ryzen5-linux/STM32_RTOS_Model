/**
  ******************************************************************************
  * @file    FreeRTOSConfig.h
  * @brief   FreeRTOS 配置文件 - 针对 STM32F103C8T6 优化
  * @author  Gemini
  * @date    2026-01-07
  ******************************************************************************
  * @attention
  * 本配置文件针对以下硬件环境：
  * - MCU: STM32F103C8T6 (72MHz, 20KB RAM, 64KB Flash)
  * - 系统时钟: 72MHz
  * - 堆大小: 8KB (可根据实际需求调整)
  ******************************************************************************
  */

#ifndef FREERTOS_CONFIG_H
#define FREERTOS_CONFIG_H

/*-----------------------------------------------------------
 * 系统时钟和节拍配置
 *-----------------------------------------------------------*/
#define configUSE_PREEMPTION                    1           /* 使用抢占式调度器 */
#define configUSE_TIME_SLICING                  1           /* 使能时间片调度 */
#define configUSE_IDLE_HOOK                     1           /* 使用空闲钩子函数 */
#define configUSE_TICK_HOOK                     0           /* 不使用节拍钩子函数 */
#define configCPU_CLOCK_HZ                      (72000000)  /* CPU时钟频率: 72MHz */
#define configTICK_RATE_HZ                      (1000)      /* RTOS节拍频率: 1000Hz (1ms) */
#define configMAX_PRIORITIES                    (5)         /* 最大任务优先级数量 */
#define configMINIMAL_STACK_SIZE                (128)       /* 空闲任务最小堆栈大小 (words) */
#define configTOTAL_HEAP_SIZE                   (12*1024)   /* FreeRTOS堆大小: 12KB */
#define configMAX_TASK_NAME_LEN                 (16)        /* 任务名称最大长度 */

/*-----------------------------------------------------------
 * 调试和统计功能配置
 *-----------------------------------------------------------*/
#define configUSE_16_BIT_TICKS                  0           /* 使用32位tick计数器 */
#define configUSE_MUTEXES                       1           /* 使能互斥量 */
#define configUSE_RECURSIVE_MUTEXES             1           /* 使能递归互斥量 */
#define configUSE_COUNTING_SEMAPHORES           1           /* 使能计数信号量 */
#define configUSE_TRACE_FACILITY                1           /* 使能任务跟踪 */
#define configGENERATE_RUN_TIME_STATS           0           /* 不生成运行时统计 */
#define configUSE_MALLOC_FAILED_HOOK            1           /* 内存分配失败钩子 */
#define configCHECK_FOR_STACK_OVERFLOW          2           /* 栈溢出检测(高水位) */

/*-----------------------------------------------------------
 * 任务管理配置
 *-----------------------------------------------------------*/
#define configQUEUE_REGISTRY_SIZE               8           /* 队列注册表大小 */
#define configUSE_APPLICATION_TASK_TAG          0           /* 不使用任务标签 */

/*-----------------------------------------------------------
 * 内存分配方案配置
 *-----------------------------------------------------------*/
#define configSUPPORT_STATIC_ALLOCATION         0           /* 不支持静态内存分配 */
#define configSUPPORT_DYNAMIC_ALLOCATION        1           /* 支持动态内存分配 */

/*-----------------------------------------------------------
 * 协程配置（本项目不使用）
 *-----------------------------------------------------------*/
#define configUSE_CO_ROUTINES                   0           /* 不使用协程 */
#define configMAX_CO_ROUTINE_PRIORITIES         (2)         /* 协程优先级数量 */

/*-----------------------------------------------------------
 * 软件定时器配置
 *-----------------------------------------------------------*/
#define configUSE_TIMERS                        1           /* 使能软件定时器（已加入timers.c） */
#define configTIMER_TASK_PRIORITY               (3)         /* 定时器任务优先级 */
#define configTIMER_QUEUE_LENGTH                10          /* 定时器命令队列长度 */
#define configTIMER_TASK_STACK_DEPTH            (256)       /* 定时器任务堆栈深度 */

/*-----------------------------------------------------------
 * FreeRTOS API 函数配置
 *-----------------------------------------------------------*/
#define INCLUDE_vTaskPrioritySet                1           /* 任务优先级设置 */
#define INCLUDE_uxTaskPriorityGet               1           /* 任务优先级获取 */
#define INCLUDE_vTaskDelete                     1           /* 任务删除 */
#define INCLUDE_vTaskCleanUpResources           0           /* 任务清理 */
#define INCLUDE_vTaskSuspend                    1           /* 任务挂起 */
#define INCLUDE_vTaskDelayUntil                 1           /* 绝对延时 */
#define INCLUDE_vTaskDelay                      1           /* 相对延时 */
#define INCLUDE_eTaskGetState                   1           /* 获取任务状态 */
#define INCLUDE_xTimerPendFunctionCall          1           /* 定时器挂起函数调用 */
#define INCLUDE_uxTaskGetStackHighWaterMark     1           /* 获取堆栈高水位 */

/*-----------------------------------------------------------
 * Cortex-M3 中断配置
 *-----------------------------------------------------------*/
#ifdef __NVIC_PRIO_BITS
    #define configPRIO_BITS                     __NVIC_PRIO_BITS
#else
    #define configPRIO_BITS                     4           /* STM32F103 使用4位中断优先级 */
#endif

/* 最低中断优先级 */
#define configLIBRARY_LOWEST_INTERRUPT_PRIORITY         0x0F

/* 内核中断优先级（最低） */
#define configKERNEL_INTERRUPT_PRIORITY                 (configLIBRARY_LOWEST_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/* 可被FreeRTOS管理的最高中断优先级 */
#define configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY    5

/* 系统调用最高中断优先级 */
#define configMAX_SYSCALL_INTERRUPT_PRIORITY            (configLIBRARY_MAX_SYSCALL_INTERRUPT_PRIORITY << (8 - configPRIO_BITS))

/*-----------------------------------------------------------
 * 中断服务函数重定义（适配 STM32 HAL 库）
 *-----------------------------------------------------------*/
#define vPortSVCHandler                         SVC_Handler
#define xPortPendSVHandler                      PendSV_Handler
#define xPortSysTickHandler                     SysTick_Handler

/*-----------------------------------------------------------
 * 断言和错误处理
 *-----------------------------------------------------------*/
#define configASSERT(x)                         if((x) == 0) {taskDISABLE_INTERRUPTS(); for(;;);}

#endif /* FREERTOS_CONFIG_H */
