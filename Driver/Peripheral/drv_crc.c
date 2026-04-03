/**
  ******************************************************************************
  * @file    drv_crc.c
  * @brief   CRC计算单元驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_crc.h"

void DRV_CRC_Init(void)
{
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_CRC, ENABLE);
}

void DRV_CRC_Reset(void)
{
    CRC_ResetDR();
}

uint32_t DRV_CRC_Calculate(const uint32_t *buf, uint32_t len)
{
    CRC_ResetDR();
    return CRC_CalcBlockCRC((uint32_t *)buf, len);
}

uint32_t DRV_CRC_Accumulate(const uint32_t *buf, uint32_t len)
{
    return CRC_CalcBlockCRC((uint32_t *)buf, len);
}

uint32_t DRV_CRC_GetResult(void)
{
    return CRC_GetCRC();
}

void DRV_CRC_WriteIDR(uint8_t data)
{
    CRC_SetIDRegister(data);
}

uint8_t DRV_CRC_ReadIDR(void)
{
    return CRC_GetIDRegister();
}
