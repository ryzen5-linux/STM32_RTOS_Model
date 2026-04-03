/**
  ******************************************************************************
  * @file    drv_dma.c
  * @brief   DMA外设驱动实现 - STM32F103C8T6
  ******************************************************************************
  */

#include "drv_dma.h"
#include <stddef.h>

void DRV_DMA_Init(const DMA_Cfg_t *cfg)
{
    DMA_InitTypeDef init;

    if (cfg == NULL) { return; }

    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

    DMA_DeInit(cfg->channel);

    /* 方向映射 */
    if (cfg->dir == DMA_DIR_MemToPeriph)
    {
        init.DMA_DIR = DMA_DIR_PeripheralDST;
    }
    else
    {
        init.DMA_DIR = DMA_DIR_PeripheralSRC;
    }

    init.DMA_PeripheralBaseAddr  = cfg->periphAddr;
    init.DMA_MemoryBaseAddr      = cfg->memAddr;
    init.DMA_BufferSize          = (uint32_t)cfg->dataLen;
    init.DMA_PeripheralInc       = (cfg->periphInc == ENABLE || cfg->dir == DMA_DIR_MemToMem)
                                   ? DMA_PeripheralInc_Enable : DMA_PeripheralInc_Disable;
    init.DMA_MemoryInc           = DMA_MemoryInc_Enable;
    init.DMA_PeripheralDataSize  = cfg->periphDataSz;
    init.DMA_MemoryDataSize      = cfg->memDataSz;
    init.DMA_Mode                = cfg->mode;
    init.DMA_Priority            = cfg->priority;
    init.DMA_M2M                 = (cfg->dir == DMA_DIR_MemToMem)
                                   ? DMA_M2M_Enable : DMA_M2M_Disable;

    DMA_Init(cfg->channel, &init);
}

void DRV_DMA_DeInit(DMA_Channel_TypeDef *ch)
{
    DMA_Cmd(ch, DISABLE);
    DMA_DeInit(ch);
}

void DRV_DMA_Enable(DMA_Channel_TypeDef *ch)
{
    DMA_Cmd(ch, ENABLE);
}

void DRV_DMA_Disable(DMA_Channel_TypeDef *ch)
{
    DMA_Cmd(ch, DISABLE);
}

void DRV_DMA_Restart(DMA_Channel_TypeDef *ch, uint16_t count)
{
    DMA_Cmd(ch, DISABLE);
    DMA_SetCurrDataCounter(ch, count);
    DMA_Cmd(ch, ENABLE);
}

uint16_t DRV_DMA_GetRemainCount(DMA_Channel_TypeDef *ch)
{
    return DMA_GetCurrDataCounter(ch);
}

void DRV_DMA_ITConfig(DMA_Channel_TypeDef *ch, uint32_t it, FunctionalState en)
{
    DMA_ITConfig(ch, it, en);
}

FlagStatus DRV_DMA_GetFlag(uint32_t flag)
{
    return DMA_GetFlagStatus(flag);
}

void DRV_DMA_ClearFlag(uint32_t flag)
{
    DMA_ClearFlag(flag);
}
