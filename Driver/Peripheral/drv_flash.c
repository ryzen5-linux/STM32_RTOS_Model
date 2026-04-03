/**
  ******************************************************************************
  * @file    drv_flash.c
  * @brief   内部Flash编程驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_flash.h"

void DRV_FLASH_Unlock(void)
{
    FLASH_Unlock();
}

void DRV_FLASH_Lock(void)
{
    FLASH_Lock();
}

int DRV_FLASH_ErasePage(uint32_t pageAddr)
{
    FLASH_Status status;

    /* 对齐到页起始地址 */
    uint32_t aligned = pageAddr & ~(DRV_FLASH_PAGE_SIZE - 1U);

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);

    status = FLASH_ErasePage(aligned);

    if (status == FLASH_COMPLETE) { return DRV_FLASH_OK; }
    if (status == FLASH_TIMEOUT)  { return DRV_FLASH_TIMEOUT; }
    return DRV_FLASH_ERROR;
}

int DRV_FLASH_WriteHalfWord(uint32_t addr, uint16_t value)
{
    FLASH_Status status;

    FLASH_ClearFlag(FLASH_FLAG_EOP | FLASH_FLAG_PGERR | FLASH_FLAG_WRPRTERR);
    status = FLASH_ProgramHalfWord(addr, value);

    if (status == FLASH_COMPLETE) { return DRV_FLASH_OK; }
    if (status == FLASH_TIMEOUT)  { return DRV_FLASH_TIMEOUT; }
    return DRV_FLASH_ERROR;
}

int DRV_FLASH_WriteWord(uint32_t addr, uint32_t value)
{
    int ret;
    ret = DRV_FLASH_WriteHalfWord(addr,       (uint16_t)(value & 0xFFFFU));
    if (ret != DRV_FLASH_OK) { return ret; }
    return DRV_FLASH_WriteHalfWord(addr + 2U, (uint16_t)(value >> 16U));
}

int DRV_FLASH_WriteBuffer(uint32_t addr, const uint16_t *buf, uint32_t count)
{
    uint32_t i;
    int ret;

    for (i = 0U; i < count; i++)
    {
        ret = DRV_FLASH_WriteHalfWord(addr + i * 2U, buf[i]);
        if (ret != DRV_FLASH_OK) { return ret; }
    }
    return DRV_FLASH_OK;
}

uint16_t DRV_FLASH_ReadHalfWord(uint32_t addr)
{
    return *((volatile uint16_t *)addr);
}

uint32_t DRV_FLASH_ReadWord(uint32_t addr)
{
    return *((volatile uint32_t *)addr);
}

void DRV_FLASH_ReadBuffer(uint32_t addr, uint8_t *buf, uint32_t len)
{
    uint32_t i;
    for (i = 0U; i < len; i++)
    {
        buf[i] = *((volatile uint8_t *)(addr + i));
    }
}

FLASH_Status DRV_FLASH_GetStatus(void)
{
    return FLASH_GetStatus();
}
