#include "stm32f10x.h"
#include "FreeRTOS.h"
#include "task.h"
#include "delay.h"

/* DWT cycle counter registers (Cortex-M3). */
#define DWT_CTRL_REG        (*(volatile uint32_t *)0xE0001000U)
#define DWT_CYCCNT_REG      (*(volatile uint32_t *)0xE0001004U)
#define DEMCR_REG           (*(volatile uint32_t *)0xE000EDFCU)
#define DEMCR_TRCENA        (1UL << 24)
#define DWT_CTRL_CYCCNTENA  (1UL << 0)

int delay_rtos_ms(u32 nms)
{
    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    {
        return 0;
    }

    if (nms == 0U)
    {
        taskYIELD();
        return 1;
    }

    vTaskDelay(pdMS_TO_TICKS(nms));
    return 1;
}

int delay_rtos_us(u32 nus)
{
    static uint8_t dwt_ready = 0U;
    uint32_t start;
    uint32_t cycles;

    if (xTaskGetSchedulerState() == taskSCHEDULER_NOT_STARTED)
    {
        return 0;
    }

    if (dwt_ready == 0U)
    {
        DEMCR_REG |= DEMCR_TRCENA;
        DWT_CYCCNT_REG = 0U;
        DWT_CTRL_REG |= DWT_CTRL_CYCCNTENA;
        dwt_ready = 1U;
    }

    cycles = (SystemCoreClock / 1000000U) * nus;
    start = DWT_CYCCNT_REG;
    while ((DWT_CYCCNT_REG - start) < cycles)
    {
    }

    return 1;
}
