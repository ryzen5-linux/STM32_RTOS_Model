/**
  ******************************************************************************
  * @file    drv_rcc.h
  * @brief   复位与时钟控制(RCC)驱动头文件
  *
  * 功能概述:
  *   - 系统时钟配置（72MHz HSE+PLL标准配置）
  *   - AHB/APB1/APB2外设时钟使能与禁止
  *   - 系统时钟频率查询
  *   - 各总线时钟频率查询
  *   - 外设软件复位
  *   - MCO时钟输出配置
  *
  * 时钟树说明（典型72MHz配置）:
  *   HSE  = 8MHz（外部晶振）
  *   PLL  = HSE * 9 = 72MHz（SYSCLK）
  *   AHB  = SYSCLK / 1 = 72MHz（HCLK）
  *   APB1 = HCLK / 2 = 36MHz（APB1CLK，外设最大36MHz）
  *   APB2 = HCLK / 1 = 72MHz（APB2CLK）
  *   ADC  = APB2 / 6 = 12MHz（ADC最大14MHz）
  *   USB  = SYSCLK / 1.5 = 48MHz（USB固定48MHz）
  ******************************************************************************
  */

#ifndef __DRV_RCC_H
#define __DRV_RCC_H

#include "stm32f10x.h"
#include "stm32f10x_gpio.h"
#include "stm32f10x_rcc.h"
#include "stm32f10x_flash.h"

/*===========================================================================
 * 数据结构
 *===========================================================================*/

/** 系统时钟频率结构体 */
typedef struct
{
    uint32_t sysclk;  /**< 系统时钟（Hz）*/
    uint32_t hclk;    /**< AHB总线时钟（Hz）*/
    uint32_t pclk1;   /**< APB1总线时钟（Hz）*/
    uint32_t pclk2;   /**< APB2总线时钟（Hz）*/
    uint32_t adcclk;  /**< ADC时钟（Hz）*/
} RCC_ClockFreq_t;

/*===========================================================================
 * API 函数声明
 *===========================================================================*/

/**
 * @brief  配置系统时钟到当前芯片组支持的标准高速档
 *         F101/F102/F103: HSE 8MHz + PLL x9 -> 72MHz
 *         F100 Value Line: HSE 8MHz / 2 * 6 -> 24MHz
 *         F105/F107: HSE 25MHz + PLL2/PREDIV1 + PLL
 *         结果时钟会随芯片组不同而变化，因此保留旧函数名仅为兼容现有接口
 *         本函数应在startup之后最早期调用（通常由SystemInit调用）
 * @note   若不使用本函数，可直接依赖system_stm32f10x.c中的SystemInit
 */
void DRV_RCC_SystemClockConfig_72MHz(void);

/**
 * @brief  获取各总线时钟频率
 * @param  freq  时钟频率结构体指针（输出）
 */
void DRV_RCC_GetClockFreq(RCC_ClockFreq_t *freq);

/**
 * @brief  获取系统时钟频率（Hz）
 * @retval SYSCLK频率
 */
uint32_t DRV_RCC_GetSysClockFreq(void);

/**
 * @brief  使能APB2外设时钟
 * @param  periph  外设掩码，如 RCC_APB2Periph_GPIOA, 可用 | 组合多个
 */
void DRV_RCC_APB2ClockEnable(uint32_t periph);

/**
 * @brief  禁止APB2外设时钟
 * @param  periph  外设掩码
 */
void DRV_RCC_APB2ClockDisable(uint32_t periph);

/**
 * @brief  使能APB1外设时钟
 * @param  periph  外设掩码，如 RCC_APB1Periph_TIM2
 */
void DRV_RCC_APB1ClockEnable(uint32_t periph);

/**
 * @brief  禁止APB1外设时钟
 * @param  periph  外设掩码
 */
void DRV_RCC_APB1ClockDisable(uint32_t periph);

/**
 * @brief  使能AHB外设时钟（DMA/CRC/FSMC等）
 * @param  periph  外设掩码，如 RCC_AHBPeriph_DMA1
 */
void DRV_RCC_AHBClockEnable(uint32_t periph);

/**
 * @brief  禁止AHB外设时钟
 * @param  periph  外设掩码
 */
void DRV_RCC_AHBClockDisable(uint32_t periph);

/**
 * @brief  软件复位APB2外设
 * @param  periph  外设掩码
 */
void DRV_RCC_APB2Reset(uint32_t periph);

/**
 * @brief  软件复位APB1外设
 * @param  periph  外设掩码
 */
void DRV_RCC_APB1Reset(uint32_t periph);

/**
 * @brief  配置MCO引脚（PA8）时钟输出
 * @param  src  时钟源：RCC_MCO_NoClock / RCC_MCO_SYSCLK /
 *              RCC_MCO_HSI / RCC_MCO_HSE / RCC_MCO_PLLCLK_Div2
 */
void DRV_RCC_MCOConfig(uint8_t src);

/**
 * @brief  查询复位原因标志
 * @retval RCC_CSR 复位标志寄存器值（包含 RCC_FLAG_SFTRST、RCC_FLAG_IWDGRST 等）
 */
uint32_t DRV_RCC_GetResetFlags(void);

/**
 * @brief  清除复位标志
 */
void DRV_RCC_ClearResetFlags(void);

#endif /* __DRV_RCC_H */
