/**
  ******************************************************************************
  * @file    app.c
  * @brief   RTOS应用层初始化 - STM32F103C8T6 用户模板
  * @date    2026-04-02
  ******************************************************************************
  */

#include "app.h"
#include "app_debug.h"
#include "tasks/task1.h"
#include "tasks/task2.h"
#include "tasks/task3.h"

SemaphoreHandle_t g_mutex_shared = NULL;
QueueHandle_t     g_queue_msg    = NULL;
SemaphoreHandle_t g_sem_event    = NULL;

static void App_CreateSyncObjects(void)
{
    g_mutex_shared = xSemaphoreCreateMutex();
    g_sem_event    = xSemaphoreCreateBinary();
    g_queue_msg    = xQueueCreate(QUEUE_SIZE_MSG, sizeof(AppMessage_t));

    if (g_mutex_shared == NULL)
    {
        Debug_Log("FAIL: g_mutex_shared");
    }
    if (g_sem_event == NULL)
    {
        Debug_Log("FAIL: g_sem_event");
    }
    if (g_queue_msg == NULL)
    {
        Debug_Log("FAIL: g_queue_msg");
    }
}

static void App_CreateTasks(void)
{
#if ENABLE_TASK1
    if (xTaskCreate(Task1, "Task1",
                    TASK_STACK_SIZE_TASK1, NULL,
                    TASK1_PRIORITY, NULL) != pdPASS)
    {
        Debug_Log("FAIL: Task1");
    }
    else
    {
        Debug_Log("OK: Task1");
    }
#endif

#if ENABLE_TASK2
    if (xTaskCreate(Task2, "Task2",
                    TASK_STACK_SIZE_TASK2, NULL,
                    TASK2_PRIORITY, NULL) != pdPASS)
    {
        Debug_Log("FAIL: Task2");
    }
    else
    {
        Debug_Log("OK: Task2");
    }
#endif

#if ENABLE_TASK3
    if (xTaskCreate(Task3, "Task3",
                    TASK_STACK_SIZE_TASK3, NULL,
                    TASK3_PRIORITY, NULL) != pdPASS)
    {
        Debug_Log("FAIL: Task3");
    }
    else
    {
        Debug_Log("OK: Task3");
    }
#endif
}

void App_Init(void)
{
    Debug_Log("App: init sync objects");
    App_CreateSyncObjects();

    Debug_Log("App: init tasks");
    App_CreateTasks();
}
