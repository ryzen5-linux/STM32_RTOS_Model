/**
  ******************************************************************************
  * @file    drv_pwr.c
  * @brief   电源管理(PWR)驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_pwr.h"

void DRV_PWR_EnterSleepMode(void)
{
    /* 清除SLEEPDEEP位，确保进入的是Sleep而非Deep-Sleep */
    SCB->SCR &= (uint32_t)~((uint32_t)SCB_SCR_SLEEPDEEP_Msk);
    __WFI();
}

void DRV_PWR_EnterStopMode(uint32_t regulator, uint8_t entry)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_EnterSTOPMode(regulator, entry);
    /* 唤醒后此处继续执行，时钟已降为HSI */
}

void DRV_PWR_EnterStandbyMode(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    DRV_PWR_ClearStandbyFlag();
    PWR_EnterSTANDBYMode();
    /* 此处不会被执行，待机模式唤醒后系统复位 */
    while (1) {}
}

void DRV_PWR_RestoreClockAfterStop(void)
{
    /* 重新使能HSE */
    RCC_HSEConfig(RCC_HSE_ON);
    while (RCC_GetFlagStatus(RCC_FLAG_HSERDY) == RESET) {}

    /* 重新使能PLL */
    RCC_PLLCmd(ENABLE);
    while (RCC_GetFlagStatus(RCC_FLAG_PLLRDY) == RESET) {}

    /* 切换系统时钟至PLL */
    RCC_SYSCLKConfig(RCC_SYSCLKSource_PLLCLK);
    while (RCC_GetSYSCLKSource() != 0x08U) {}
}

void DRV_PWR_PVDConfig(uint32_t level, FunctionalState en)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_PVDLevelConfig(level);
    PWR_PVDCmd(en);
}

FlagStatus DRV_PWR_GetPVDOutput(void)
{
    return PWR_GetFlagStatus(PWR_FLAG_PVDO);
}

void DRV_PWR_ClearStandbyFlag(void)
{
    PWR_ClearFlag(PWR_FLAG_SB);
    PWR_ClearFlag(PWR_FLAG_WU);
}

FlagStatus DRV_PWR_GetStandbyFlag(void)
{
    return PWR_GetFlagStatus(PWR_FLAG_SB);
}

void DRV_PWR_WakeUpPinCmd(FunctionalState en)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR, ENABLE);
    PWR_WakeUpPinCmd(en);
}
