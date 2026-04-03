/**
  ******************************************************************************
  * @file    drv_dma.h
  * @brief   DMA外设驱动头文件 - STM32F103C8T6
  *
  * 功能概述:
  *   - STM32F103C8T6 内置 DMA1（7通道）
  *   - 支持 MEM->MEM、外设->MEM、MEM->外设传输
  *   - 循环模式与单次模式
  *   - 传输完成/半完成/错误中断配置
  *   - 剩余传输计数查询
  *
  * DMA1通道与外设对应关系（部分常用）:
  *   CH1: ADC1
  *   CH2: USART3_TX / TIM1_CH1 / SPI1_RX / TIM3_CH3 / TIM3_UP
  *   CH3: USART3_RX / TIM1_CH2 / SPI1_TX / TIM3_CH4 / TIM3_TRIG
  *   CH4: USART1_TX / TIM1_CH4 / SPI2_RX / I2C2_TX / TIM4_CH2
  *   CH5: USART1_RX / TIM1_UP  / SPI2_TX / I2C2_RX / TIM4_CH3
  *   CH6: USART2_RX / TIM1_CH3 / I2C1_TX / TIM3_CH1 / TIM4_UP
  *   CH7: USART2_TX / TIM1_COM / I2C1_RX / TIM3_CH2 / TIM4_CH1
  *
  * 使用示例（内存拷贝）:
  *   DMA_Cfg_t cfg = { DMA1_Channel1, DMA_DIR_MemToMem,
  *                     DMA_Mode_Normal, DMA_Priority_High,
  *                     DMA_PeripheralDataSize_Byte, DMA_MemoryDataSize_Byte,
  *                     DISABLE, srcBuf, dstBuf, 64 };
  *   DRV_DMA_Init(&cfg);
  *   DRV_DMA_Enable(DMA1_Channel1);
  ******************************************************************************
  */

#ifndef __DRV_DMA_H
#define __DRV_DMA_H

#include "stm32f10x.h"
#include "stm32f10x_dma.h"

/*===========================================================================
 * 枚举与宏
 *===========================================================================*/

/** DMA传输方向 */
typedef enum
{
    DMA_DIR_PeriphToMem = 0U, /**< 外设 -> 内存 */
    DMA_DIR_MemToPeriph = 1U, /**< 内存 -> 外设 */
    DMA_DIR_MemToMem    = 2U  /**< 内存 -> 内存（仅单次模式） */
} DMA_Dir_t;

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** DMA通道初始化配置结构体 */
typedef struct
{
    DMA_Channel_TypeDef *channel;       /**< DMA通道，如 DMA1_Channel1 */
    DMA_Dir_t            dir;           /**< 传输方向 */
    uint32_t             mode;          /**< 传输模式：DMA_Mode_Normal / DMA_Mode_Circular */
    uint32_t             priority;      /**< 优先级：DMA_Priority_Low/Medium/High/VeryHigh */
    uint32_t             periphDataSz;  /**< 外设数据宽度：DMA_PeripheralDataSize_Byte/HalfWord/Word */
    uint32_t             memDataSz;     /**< 内存数据宽度：DMA_MemoryDataSize_Byte/HalfWord/Word */
    FunctionalState      periphInc;     /**< 外设地址自增：ENABLE / DISABLE */
    uint32_t             periphAddr;    /**< 外设/源内存地址 */
    uint32_t             memAddr;       /**< 内存/目的地址 */
    uint16_t             dataLen;       /**< 传输数据个数（单位：periphDataSz） */
} DMA_Cfg_t;

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  初始化DMA通道
 * @param  cfg  配置结构体指针
 */
void DRV_DMA_Init(const DMA_Cfg_t *cfg);

/**
 * @brief  关闭并复位DMA通道
 * @param  ch  DMA通道
 */
void DRV_DMA_DeInit(DMA_Channel_TypeDef *ch);

/**
 * @brief  使能DMA通道，启动传输
 * @param  ch  DMA通道
 */
void DRV_DMA_Enable(DMA_Channel_TypeDef *ch);

/**
 * @brief  禁止DMA通道
 * @param  ch  DMA通道
 */
void DRV_DMA_Disable(DMA_Channel_TypeDef *ch);

/**
 * @brief  重新设置传输计数并启动（用于循环外重启）
 * @param  ch      DMA通道
 * @param  count   新的传输数据个数
 */
void DRV_DMA_Restart(DMA_Channel_TypeDef *ch, uint16_t count);

/**
 * @brief  获取剩余传输数据个数
 * @param  ch  DMA通道
 * @retval 剩余数据个数（CNDTR寄存器值）
 */
uint16_t DRV_DMA_GetRemainCount(DMA_Channel_TypeDef *ch);

/**
 * @brief  使能/禁止DMA通道中断
 * @param  ch    DMA通道
 * @param  it    中断源，如 DMA_IT_TC / DMA_IT_HT / DMA_IT_TE
 * @param  en    ENABLE / DISABLE
 */
void DRV_DMA_ITConfig(DMA_Channel_TypeDef *ch, uint32_t it, FunctionalState en);

/**
 * @brief  查询DMA中断标志（通过DMA1全局ISR寄存器判断）
 * @param  flag  DMA1_FLAG_TC1 / DMA1_FLAG_HT1 / DMA1_FLAG_TE1 ... 等
 * @retval SET / RESET
 */
FlagStatus DRV_DMA_GetFlag(uint32_t flag);

/**
 * @brief  清除DMA中断标志
 * @param  flag  待清除标志
 */
void DRV_DMA_ClearFlag(uint32_t flag);

#endif /* __DRV_DMA_H */
