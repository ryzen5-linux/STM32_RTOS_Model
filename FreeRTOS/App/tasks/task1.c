/**
  ******************************************************************************
  * @file    task1.c
  * @brief   RTOS模板任务1实现
  ******************************************************************************
  */

#include "task1.h"
#include "app.h"
#include "app_debug.h"

void Task1(void *argument)
{
    TickType_t lastWakeTime;
    AppMessage_t msg;

    (void)argument;
    lastWakeTime = xTaskGetTickCount();
    msg.id = 1U;
    msg.value = 0U;

    Debug_Log("Task1 start");

    for (;;)
    {
        msg.value++;

        if (g_queue_msg != NULL)
        {
            (void)xQueueSend(g_queue_msg, &msg, 0U);
        }

        vTaskDelayUntil(&lastWakeTime, pdMS_TO_TICKS(TASK_PERIOD_TASK1));
    }
}
