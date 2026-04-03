/**
  ******************************************************************************
  * @file    task3.c
  * @brief   RTOS模板任务3实现
  ******************************************************************************
  */

#include "task3.h"
#include "app.h"
#include "app_debug.h"
#include <stdio.h>

void Task3(void *argument)
{
    uint32_t eventCount = 0U;
    uint32_t heartbeatCount = 0U;
    static char out[96];
    UBaseType_t queueDepth;
    UBaseType_t taskCount;
    size_t freeHeap;
    size_t minFreeHeap;

    (void)argument;
    Debug_Log("Task3 start");

    for (;;)
    {
        queueDepth = (g_queue_msg != NULL) ? uxQueueMessagesWaiting(g_queue_msg) : 0U;
        taskCount = uxTaskGetNumberOfTasks();
        freeHeap = xPortGetFreeHeapSize();
        minFreeHeap = xPortGetMinimumEverFreeHeapSize();

        if (g_sem_event != NULL)
        {
            if (xSemaphoreTake(g_sem_event, pdMS_TO_TICKS(TASK_PERIOD_TASK3)) == pdPASS)
            {
                eventCount++;
                (void)snprintf(out, sizeof(out), "RTOS semaphore ok: count=%lu",
                               (unsigned long)eventCount);
                Debug_Log(out);
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_TASK3));
        }

        heartbeatCount++;
        (void)snprintf(out, sizeof(out),
                       "Heartbeat: count=%lu tick=%lu tasks=%lu queue=%lu heap=%lu minHeap=%lu",
                       (unsigned long)heartbeatCount,
                       (unsigned long)xTaskGetTickCount(),
                       (unsigned long)taskCount,
                       (unsigned long)queueDepth,
                       (unsigned long)freeHeap,
                       (unsigned long)minFreeHeap);
        Debug_Log(out);
    }
}
