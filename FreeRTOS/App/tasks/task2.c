/**
  ******************************************************************************
  * @file    task2.c
  * @brief   RTOS模板任务2实现
  ******************************************************************************
  */

#include "task2.h"
#include "app.h"
#include "app_debug.h"
#include <stdio.h>

void Task2(void *argument)
{
    AppMessage_t msg;
    char out[64];

    (void)argument;
    Debug_Log("Task2 start");

    for (;;)
    {
        if (g_queue_msg != NULL)
        {
            if (xQueueReceive(g_queue_msg, &msg, pdMS_TO_TICKS(TASK_PERIOD_TASK2)) == pdPASS)
            {
                (void)snprintf(out, sizeof(out), "RTOS queue ok: id=%lu value=%lu",
                               (unsigned long)msg.id,
                               (unsigned long)msg.value);
                Debug_Log(out);

                if (g_mutex_shared != NULL)
                {
                    if (xSemaphoreTake(g_mutex_shared, pdMS_TO_TICKS(10U)) == pdPASS)
                    {
                        Debug_Log("RTOS mutex ok");
                        xSemaphoreGive(g_mutex_shared);
                    }
                }

                if (g_sem_event != NULL)
                {
                    (void)xSemaphoreGive(g_sem_event);
                }
            }
        }
        else
        {
            vTaskDelay(pdMS_TO_TICKS(TASK_PERIOD_TASK2));
        }
    }
}
