/**
  ******************************************************************************
  * @file    drv_bkp.c
  * @brief   备份寄存器(BKP)驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_bkp.h"

/* 备份域访问必须先使能PWR和BKP时钟，并解除写保护 */
static void bkp_access_enable(void)
{
    RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);
    PWR_BackupAccessCmd(ENABLE);
}

void DRV_BKP_Write(uint16_t reg, uint16_t value)
{
    bkp_access_enable();
    BKP_WriteBackupRegister(reg, value);
}

uint16_t DRV_BKP_Read(uint16_t reg)
{
    return BKP_ReadBackupRegister(reg);
}

void DRV_BKP_ResetAll(void)
{
    static const uint16_t regs[10] = {
        BKP_DR1,  BKP_DR2,  BKP_DR3,  BKP_DR4,  BKP_DR5,
        BKP_DR6,  BKP_DR7,  BKP_DR8,  BKP_DR9,  BKP_DR10
    };
    uint8_t i;
    bkp_access_enable();
    for (i = 0U; i < 10U; i++)
    {
        BKP_WriteBackupRegister(regs[i], 0x0000U);
    }
}

void DRV_BKP_RTCOutputConfig(FunctionalState en, FunctionalState calib512)
{
    bkp_access_enable();
    if (en == ENABLE)
    {
        BKP_RTCOutputConfig((calib512 == ENABLE)
                            ? BKP_RTCOutputSource_CalibClock
                            : BKP_RTCOutputSource_Second);
    }
    else
    {
        BKP_RTCOutputConfig(BKP_RTCOutputSource_None);
    }
}

void DRV_BKP_TamperConfig(uint16_t activeLevel, FunctionalState en)
{
    bkp_access_enable();
    BKP_TamperPinLevelConfig(activeLevel);
    BKP_TamperPinCmd(en);
    BKP_ITConfig(en);
}
