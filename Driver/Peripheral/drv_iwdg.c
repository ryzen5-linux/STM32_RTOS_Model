/**
  ******************************************************************************
  * @file    drv_iwdg.c
  * @brief   独立看门狗(IWDG)驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_iwdg.h"

/* LSI典型频率 40kHz */
#define IWDG_LSI_FREQ_HZ  (40000U)

void DRV_IWDG_Init(uint32_t timeoutMs)
{
    /* 分频表：{分频倍数, IWDG_Prescaler_x} */
    static const uint32_t psc_vals[] = {4U, 8U, 16U, 32U, 64U, 128U, 256U};
    static const uint8_t  psc_regs[] = {
        IWDG_Prescaler_4,   IWDG_Prescaler_8,  IWDG_Prescaler_16,
        IWDG_Prescaler_32,  IWDG_Prescaler_64, IWDG_Prescaler_128,
        IWDG_Prescaler_256
    };
    uint8_t  i;
    uint32_t reload = 0U;
    uint8_t  psc_reg = IWDG_Prescaler_256;

    /* 从小分频开始，找到能满足超时且reload <= 4095的配置 */
    for (i = 0U; i < 7U; i++)
    {
        /* reload = timeoutMs * LSI_FREQ / psc / 1000 */
        reload = timeoutMs * (IWDG_LSI_FREQ_HZ / psc_vals[i]) / 1000U;
        if (reload <= 4095U && reload >= 1U)
        {
            psc_reg = psc_regs[i];
            break;
        }
        reload = 4095U; /* 保底值 */
        psc_reg = IWDG_Prescaler_256;
    }

    IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);
    IWDG_SetPrescaler(psc_reg);
    IWDG_SetReload((uint16_t)reload);

    /* 等待预分频和重装值生效 */
    while (IWDG_GetFlagStatus(IWDG_FLAG_PVU) != RESET) {}
    while (IWDG_GetFlagStatus(IWDG_FLAG_RVU) != RESET) {}

    IWDG_ReloadCounter();
    IWDG_Enable();
}

void DRV_IWDG_Feed(void)
{
    IWDG_ReloadCounter();
}
