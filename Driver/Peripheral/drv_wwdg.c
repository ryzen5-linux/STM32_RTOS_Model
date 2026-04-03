/**
  ******************************************************************************
  * @file    drv_wwdg.c
  * @brief   窗口看门狗(WWDG)驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_wwdg.h"

void DRV_WWDG_Init(uint8_t prescaler, uint8_t counter, uint8_t windowVal)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_WWDG, ENABLE);

    WWDG_SetPrescaler(prescaler);
    WWDG_SetWindowValue(windowVal);
    WWDG_Enable(counter);
}

void DRV_WWDG_Feed(uint8_t counter)
{
    WWDG_SetCounter(counter);
}

void DRV_WWDG_EWIConfig(FunctionalState en)
{
    if (en == ENABLE)
    {
        WWDG_EnableIT();
    }
    /* WWDG EWI一经使能即不可软件禁止，仅清除标志 */
}

void DRV_WWDG_ClearEWIFlag(void)
{
    WWDG_ClearFlag();
}
